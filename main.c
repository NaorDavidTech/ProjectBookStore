//Naor David 

#define _CRT_SECURE_NO_WARNINGS
#include "bookstore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


int main(int argc, char* argv[])
{
    // Allow optional file paths from command line
    const char* usersPath = (argc > 1) ? argv[1] : "users.txt";
    const char* itemsPath = (argc > 2) ? argv[2] : "items.txt";
    const char* customerPath = (argc > 3) ? argv[3] : "customer.txt";
    const char* logPath = (argc > 4) ? argv[4] : "log.txt";

    // Initialize BookStore system
    BookStore store;
    initSystem(&store, usersPath, itemsPath, customerPath, logPath);
    
    loadBooksFromFile(&store);
    loadCustomersFromFile(&store);

    // Attempt login
    if (!handleLogin(&store)) 
    {
        printf("Login failed. Maximum attempts exceeded. Exiting...\n");
        cleanup(&store);
        return 1;
    }

    // Main menu loop
    int choice;
    int running = 1;

    while (running) 
    {
        // Display menu with current user context
        displayMenu(&store);

        // Get and validate user input
        if (!handleUserInput(&choice, &store)) 
        {
            continue;  // Invalid input, restart loop
        }

        // Process user choice
        switch (choice) 
        {
        case 0:  // Exit
            printf("Saving and exiting the Book Store Management System...\n");
            fprintf(store.logFile, "System exited by user: %s\n", store.currentUsername);
            running = 0;
            break;           
        case 1:  // View all books
            displayBooks(&store);
            break;
        case 2:  // Search books
            handleSearchBooks(&store);
            break;
        case 3:  // Add new book
            handleAddBook(&store);
            displayBooks(&store);
            break;

        case 4: // Customer Management
            handleCustomerManagement(&store);
            break;
        case 5:  // Update book
            if (store.currentUserLevel >= PERMISSION_TWO ) 
            {
                handleUpdateBook(&store);
                displayBooks(&store);
            }
            else 
            {
                printf("Access denied. Insufficient permissions.\n");
                fprintf(store.logFile, "Unauthorized update attempt by %s\n", store.currentUsername);
            }
            break;
        case 6:  // Delete book
            if (store.currentUserLevel >= PERMISSION_TWO ) 
            {
                handleDeleteBook(&store);
                displayBooks(&store);
            }
            else 
            {
                printf("Access denied. Insufficient permissions.\n");
                fprintf(store.logFile, "Unauthorized delete attempt by %s\n", store.currentUsername);
            }
            break;
        case 7:  // View users
            if (store.currentUserLevel == PERMISSION_MANAGER)
            {
                displayUsers(&store);
            }
            else
            {
                printf("Access denied. Admin permissions required.\n");
                fprintf(store.logFile, "Unauthorized users view attempt by %s\n", store.currentUsername);
            }
            break;
        case 8:  // Add new user
            if (store.currentUserLevel == PERMISSION_MANAGER)
            {
                handleAddUser(&store);
                displayUsers(&store);
            }
            else 
            {
                printf("Access denied. Admin permissions required.\n");
                fprintf(store.logFile, "Unauthorized new user creation attempt by %s\n", store.currentUsername);
            }
            break;
        case 9:  // Delete user
            if (store.currentUserLevel == PERMISSION_MANAGER)
            {
                handleDeleteUser(&store);
                displayUsers(&store);
            }
            else
            {
                printf("Access denied. Admin permissions required.\n");
                fprintf(store.logFile, "Unauthorized user deletion attempt by %s\n", store.currentUsername);
            }
            break;
        default:
            printf("Error: Invalid choice! Please try again.\n");
            fprintf(store.logFile, "Invalid menu choice entered: %d\n", choice);
        }
    }
    saveToFile(&store);

    // Cleanup system resources
    cleanup(&store);
    return 0;
}


