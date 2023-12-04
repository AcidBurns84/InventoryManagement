#include "inventory_management.h"
#include <iostream>
#include <stdexcept>

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