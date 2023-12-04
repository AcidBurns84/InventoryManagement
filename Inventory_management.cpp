#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <functional>
#include <limits>
#include <utility>
#include <tuple> 
#include <optional>
#include <map>

struct Part {
    std::string part_number;
    std::string part_description;
    double part_cost;
    double part_retail;
    std::string part_manufacturer;
    int part_quantity;
};

std::vector<Part> inventory; 

class User {
private:
    std::string username;
    std::string password;
    bool is_admin;

public:
    User(const std::string& username, const std::string& password, bool isAdmin)
        : username(username), password(password), is_admin(is_admin) {}

    // Only validate the password
    bool validate(const std::string& try_password) const {
        return password == try_password;
    }

    bool isAdministrator() const {
        return is_admin;
    }

    std::string getUsername() const {
        return username;
    }
};

class UserManager {
private:
    std::map<std::string, User> users;

public:
    UserManager() {

        // Adding some default users without encryption
        addUser("admin", "admin123", true);
        addUser("user", "user123", false);
    }

    void addUser(const std::string& username, const std::string& password, bool isAdmin) {
        users.insert(std::make_pair(username, User(username, password, isAdmin)));
    }

    User* getUser(const std::string& username) {
        auto it = users.find(username);
        if (it != users.end()) {
            return &it->second;
    }
    return nullptr;
}

};

User* login(UserManager& manager) {
    std::string username;
    std::string password;

    std::cout << "####################\n";
    std::cout << "#  LOGIN INTERFACE  #\n";
    std::cout << "####################\n";

    std::cout << "Enter username: ";
    std::cin >> username;

    std::cout << "Enter password: ";
    std::cin >> password;

    // Clear the input buffer to avoid reading the trailing newline character
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    User* user = manager.getUser(username);
    if (user && user->validate(password)) {
        std::cout << "Login successful. ";
        if (user->isAdministrator()) {
            std::cout << "Hello, Admin!" << std::endl;
        } else {
            std::cout << "Hello, User!" << std::endl;
        }
        return user;
    }
    std::cout << "Login failed. Invalid credentials." << std::endl;
    return nullptr;
}

void save_to_file(const std::string& filename) {
    std::ofstream out(filename);
    for (const auto& part : inventory) {
        out << part.part_number << "," 
            << part.part_description << "," 
            << part.part_cost << "," 
            << part.part_retail << ","
            << part.part_manufacturer << ","
            << part.part_quantity << "\n";
    }
    out.close();
}


void load_from_file(const std::string& filename) {
    std::ifstream in(filename);
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        Part part;
        std::getline(iss, part.part_number, ',');
        std::getline(iss, part.part_description, ',');
        iss >> part.part_cost;
        char comma;
        iss >> comma;  // eat the comma after cost
        iss >> part.part_retail;
        iss >> comma;  // eat the comma after retail price
        std::getline(iss, part.part_manufacturer, ',');  // Read manufacturer, but now stop at comma
        iss >> part.part_quantity;
        inventory.push_back(part);
    }
    in.close();
}

std::string to_lower(const std::string& str) {
    std::string lowerStr;
    lowerStr.resize(str.size());
    std::transform(str.begin(), str.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

void display_matching_parts(const std::vector<Part*>& matching_parts) {
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << std::left << std::setw(14) << "Part Number" << " | "
              << std::left << std::setw(22) << "Description" << " | "
              << std::left << std::setw(7) << "Cost" << " | "
              << std::left << std::setw(7) << "Retail" << " | "
              << std::left << std::setw(15) << "Manufacturer" << " | "
              << std::left << "Quantity" << std::endl;
    std::cout << "--------------------------------------------------------------------------------\n";

    for (size_t i = 0; i < matching_parts.size(); ++i) {
        const Part& part = *matching_parts[i];
        std::cout << i + 1 << ": "
                  << std::left << std::setw(14) << std::setfill(' ') << part.part_number << " | "
                  << std::left << std::setw(22) << part.part_description << " | "
                  << std::left << std::setw(7) << std::fixed << std::setprecision(2) << part.part_cost << " | "
                  << std::left << std::setw(7) << std::fixed << std::setprecision(2) << part.part_retail << " | "
                  << std::left << std::setw(22) << part.part_manufacturer << " | "
                  << std::left << part.part_quantity << std::endl;
    }
    std::cout << "--------------------------------------------------------------------------------\n";
}



void add_part() {
    //Declare a new variable of type part. This will serve as the new part.
    Part new_part;

    // Get part number and search for matches
    std::cout << "Enter part number: ";
    std::getline(std::cin, new_part.part_number);
    std::string lowerPartNumber = to_lower(new_part.part_number);
    // Declares a vector to store pointers to matching parts
    std::vector<Part*> matching_parts;
    // Begins the loop over the inventory.
    // It also checks for matching parts inside the inventory lower case.
    for (auto& part : inventory) {
        if (to_lower(part.part_number) == lowerPartNumber) {
            matching_parts.push_back(&part);
        }
    }

    if (matching_parts.empty()) {
        // If no part number match, ask for description and try to match
        std::cout << "Enter description: ";
        std::getline(std::cin, new_part.part_description);
        std::string lowerDescription = to_lower(new_part.part_description);

        for (auto& part : inventory) {
            if (to_lower(part.part_description).find(lowerDescription) != std::string::npos) {
                matching_parts.push_back(&part);
            }
        }
    }
    
    if (!matching_parts.empty()) {
        display_matching_parts(matching_parts);

        std::cout << "Select a part to update (enter number), or enter 0 to add a new part: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice > 0 && choice <= matching_parts.size()) {
            int quantityToAdd;
            std::cout << "Enter quantity to add: ";
            std::cin >> quantityToAdd;
            std::cin.ignore();
            matching_parts[choice - 1]->part_quantity += quantityToAdd;
            std::cout << "Quantity updated. Total quantity: " << matching_parts[choice - 1]->part_quantity << std::endl;
            return;
        }
    }

    // Continue to add new part details if no matching part selected
    if (new_part.part_description.empty()) {
        std::cout << "Enter description: ";
        std::getline(std::cin, new_part.part_description);
    }

    std::cout << "Enter cost: ";
    std::cin >> new_part.part_cost;
    std::cin.ignore();

    new_part.part_retail = new_part.part_cost / 0.65;  // Calculate retail price

    std::cout << "Enter manufacturer: ";
    std::getline(std::cin, new_part.part_manufacturer);

    std::cout << "Enter quantity: ";
    std::cin >> new_part.part_quantity;
    std::cin.ignore();

    inventory.push_back(new_part);
    std::cout << "New part added to inventory." << std::endl;
}



void edit_part(){
    std::cout << "Enter the part number or description of the part you wish to edit: ";
    std::string search;
    std::getline(std::cin, search);

    search = to_lower(search);

    std::vector<Part*> matching_parts;

    for (Part& part : inventory) {
        if (to_lower(part.part_number).find(search) != std::string::npos || to_lower(part.part_description).find(search) != std::string::npos) {
            matching_parts.push_back(&part);
        }
    }

    if (!matching_parts.empty()) {
        std::cout << "Matching parts found:\n";
        for (size_t i = 0; i < matching_parts.size(); ++i) {
            const Part* part = matching_parts[i];
            std::cout << (i + 1) << ". Part Number: " << part->part_number
                  << ", Description: " << part->part_description
                  << ", Manufacturer: " << part->part_manufacturer
                  << ", Cost: " << part->part_cost
                  << ", Quantity: " << part->part_quantity << "\n";
    }
        std::cout << "Enter 0 to return to the main menu.\n";
        std::cout << "Select a part to edit (1-" << matching_parts.size() << "): ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 0) {
            return;
        }

        if (choice > 0 && choice <= matching_parts.size()) {
            Part& selectedPart = *matching_parts[choice-1];
            while (true) {
                std::cout << "What would you like to edit?\n";
                std::cout << "1. Part Number\n";
                std::cout << "2. Description\n";
                std::cout << "3. Cost\n";
                std::cout << "4. Manufacturer\n";
                std::cout << "5. Quantity\n";
                std::cout << "6. Exit to main menu\n";
                std::cout << "Select an option (1-6): ";
                int choice;
                std::cin >> choice;
                std::cin.ignore();  // clear the newline character

                switch (choice) {
                    case 1:
                    {
                        std::cout << "Enter new part number: ";
                        std::getline(std::cin, selectedPart.part_number);
                        break;
                    }
                    case 2:
                    {
                        std::cout << "Enter new description: ";
                        std::getline(std::cin, selectedPart.part_description);
                        break;
                    }
                    case 3:
                    {
                        std::cout << "Enter new cost: ";
                        std::cin >> selectedPart.part_cost;
                        std::cin.ignore();  // clear the newline character
                        selectedPart.part_retail = selectedPart.part_cost / 0.65;
                        break;
                    }
                    case 4:
                    {
                        std::cout << "Enter new manufacturer: ";
                        std::getline(std::cin, selectedPart.part_manufacturer);
                        break;
                    }
                    case 5:
                    {
                        std::cout << "Enter new quantity: ";
                        std::cin >> selectedPart.part_quantity;
                        std::cin.ignore();  // clear the newline character
                        break;
                    }
                    case 6:
                        return;  // exit the function and go back to main menu
                    default:
                        std::cout << "Invalid choice. Please choose again.\n";
                }
            }
        } else {
            std::cout << "Invalid choice.\n";
        }
    } else {
        std::cout << "No part found with the given part number or description.\n";
    }
}


void delete_part() {
    while (true) {
        std::cout << "Enter the part number or description of the part you wish to delete: ";
        std::string search;
        std::getline(std::cin, search);

    search = to_lower(search);

    std::vector<int> indices_to_delete;

    for (size_t i = 0; i < inventory.size(); ++i) {
        if (to_lower(inventory[i].part_number).find(search) != std::string::npos || 
            to_lower(inventory[i].part_description).find(search) != std::string::npos) {
            indices_to_delete.push_back(i);
        }
    }

    if (!indices_to_delete.empty()) {
        std::cout << "Matching part(s) found:" << std::endl;
        for (size_t i = 0; i < indices_to_delete.size(); ++i) {
            const Part& part = inventory[indices_to_delete[i]];
            std::cout << (i + 1) << ". Part Number: " << part.part_number
                  << ", Description: " << part.part_description
                  << ", Manufacturer: " << part.part_manufacturer
                  << ", Quantity: " << part.part_quantity << std::endl;
    }

        std::cout << "Enter the number of the part you wish to delete: ";
        size_t partIndex;
        std::cin >> partIndex;
        std::cin.ignore();

        if (partIndex < 1 || partIndex > indices_to_delete.size()) {
            std::cout << "Invalid selection." << std::endl;
            continue;
        }

        size_t actualIndex = indices_to_delete[partIndex - 1];

        std::cout << "Choose an option:\n";
        std::cout << "1. Delete a specific quantity\n";
        std::cout << "2. Delete the entire part\n";
        std::cout << "3. Return to main menu\n";
        std::cout << "Enter your choice: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1: {  // Delete a specific quantity
                int quantity;
                std::cout << "Enter the quantity to delete: ";
                std::cin >> quantity;
                std::cin.ignore();

                if (quantity <= 0 || quantity > inventory[actualIndex].part_quantity) {
                    std::cout << "Invalid quantity." << std::endl;
                } else {
                    inventory[actualIndex].part_quantity -= quantity;
                    if (inventory[actualIndex].part_quantity <= 0) {
                        inventory.erase(inventory.begin() + actualIndex);
                    }
                    std::cout << "Quantity deleted." << std::endl;
                }
                break;
            }
            case 2: {  // Delete the entire part
                inventory.erase(inventory.begin() + actualIndex);
                std::cout << "Part deleted." << std::endl;
                break;
            }
            case 3: // Return to main menu
                return;
            default:
                std::cout << "Invalid option. Please try again." << std::endl;
                continue;
        }

        std::cout << "Do you want to delete more parts? (1 for yes, 0 for no): ";
        int more_choice;
        std::cin >> more_choice;
        std::cin.ignore();

        if (more_choice != 1) {
            break;
        }
    }

    std::cout << "Returning to main menu." << std::endl;
}
}

void list_parts() {
    if (inventory.empty()) {
        std::cout << "Inventory is empty." << std::endl;
        return;
    }

    std::cout << "Inventory:" << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    std::cout << std::left << std::setw(14) << "Part Number" << " | " <<
                 std::left << std::setw(22) << "Description" << " | " <<
                 std::left << std::setw(7) << "Cost" << " | " <<
                 std::left << std::setw(7) << "Retail" << " | " <<
                 std::left << std::setw(15) << "Manufacturer" << " | " <<
                 std::left << "Quantity" << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    struct {
        bool operator()(const Part& a, const Part& b) const {   
            return a.part_manufacturer < b.part_manufacturer; 
        }   
    } manufacturer_sort;
    std::sort(inventory.begin(), inventory.end(), manufacturer_sort);
    for (const Part& part : inventory) {
        std::cout << std::left << std::setw(14) << std::setfill(' ') << part.part_number << " | " <<
                     std::left << std::setw(22) << part.part_description << " | " <<
                     std::left << std::setw(7) << std::fixed << std::setprecision(2) << part.part_cost << " | " <<
                     std::left << std::setw(7) << std::fixed << std::setprecision(2) << part.part_retail << " | " <<
                     std::left << std::setw(15) << part.part_manufacturer << " | " <<
                     std::left << part.part_quantity << std::endl;
    }

    std::cout << "--------------------------------------------------------------------------------" << std::endl;
}

void search_part() {
    std::cout << "Enter a keyword to search for (part number or description): ";
    std::string keyword;
    std::getline(std::cin, keyword);

    keyword = to_lower(keyword);
    bool found = false;

    for (const auto& part : inventory) {
        if (to_lower(part.part_number).find(keyword) != std::string::npos || 
            to_lower(part.part_description).find(keyword) != std::string::npos) {
            if (!found) {
                std::cout << "Matching parts found:\n";
                found = true;
            }
            std::cout << "Part Number: " << part.part_number
                      << ", Description: " << part.part_description
                      << ", Cost: " << part.part_cost
                      << ", Retail: " << part.part_retail
                      << ", Manufacturer: " << part.part_manufacturer
                      << ", Quantity: " << part.part_quantity << "\n";
        }
    }

    if (!found) {
        std::cout << "No parts found matching the keyword." << std::endl;
    }
}

void inventory_management() {

    while (true) {
        std::cout << "Inventory Management System" << std::endl;
        std::cout << "1. Add Part\n2. Delete Part\n3. Edit Part\n4. List Parts\n5. Search\n6. Exit\n";

        std::string choice_str;
        std::getline(std::cin, choice_str);

        // Convert the string to an integer
        int choice;
        try {
            choice = std::stoi(choice_str);
        } catch (std::invalid_argument&) {
            choice = 0;  // Invalid input
        }

        switch (choice) {
            case 1:
                add_part();
                break;
            case 2:
                delete_part();
                break;
            case 3:
                edit_part();
                break;
            case 4:
                list_parts();
                break;
            case 5:
                search_part();
                break;
            case 6:
                save_to_file("data.txt"); // Save before exiting
                return;
            default:
                std::cout << "Invalid choice. Please choose one of the provided options." << std::endl;
        }
    }
}

int main() {
    load_from_file("data.txt");
    UserManager userManager;
    User* loggedInUser = login(userManager);
    if (loggedInUser) {
        inventory_management();
    } else {
        std::cout << "Exiting due to failed login." << std::endl;
    }
}
