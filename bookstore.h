//Naor David

#define _CRT_SECURE_NO_WARNINGS
#ifndef BOOKSTORE_H
#define BOOKSTORE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>


#define MAX_USERNAME 30
#define MAX_PASSWORD 15
#define MAX_FULLNAME 30
#define MAX_TITLE 60      
#define MAX_AUTHOR 40
#define MAX_GENRE 15
#define MAX_PHONE 15
#define MAX_EMAIL 50

#define LOGIN_ATTEMPTS 3
#define PERMISSION_ONE 1    // Basic user permission 
#define PERMISSION_TWO 2    // User with edit permissions
#define PERMISSION_MANAGER 3   // User with manager permissions (highest level)


typedef struct user 
{
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int level;
    char fullname[MAX_FULLNAME];
} user;


typedef struct Customer 
{
    int id;
    char name[MAX_FULLNAME];
    char phone[MAX_PHONE];
    char email[MAX_EMAIL];
    struct Customer* next;
} Customer;



typedef struct Book 
{
    int serial_number;        
    char title[MAX_TITLE];     
    char author[MAX_AUTHOR]; 
    char genre[MAX_GENRE];
    double price;           
    bool is_bestseller;       
    struct 
    {
        int day;
        int month;
        int year;
    }publish_date;
    struct Book* next;       
    struct Book* prev;       
} Book;

 

typedef struct BookStore 
{
    Book* head;
    Book* tail;
    Book* current;
    FILE* logFile;
    int currentUserLevel;
    char currentUsername[MAX_USERNAME];
    char* usersPath;
    char* itemsPath;
    char* logPath; 
    char* customerPath;
    Customer* customerHead;
} BookStore;





// Functions for system initialization and file management
void initSystem(BookStore* store, const char* users_path, const char* items_path, const char* customer_path, const char* log_path);
void initUsersFile(BookStore* store);
int checkAndCreateFiles(BookStore* store);



// Functions for user management
void getDateTime(int* day, int* month, int* year, int* hours, int* mins);
int validateUserCredentials(const char* username, const char* password, int* level, BookStore* store);
int handleLogin(BookStore* store);
void displayUsers(BookStore* store);
void handleAddUser(BookStore* store);
int checkUserExists(const char* username, BookStore* store);
void handleDeleteUser(BookStore* store);



// Functions for book management
Book* createBook(int serial, const char* title, const char* author, const char* genre, double price, bool is_bestseller, int day, int month, int year, BookStore* store);
void insertBook(BookStore* store, Book* newBook);
void loadBooksFromFile(BookStore* store);
void displayBooks(BookStore* store);
Book* findBook(BookStore* store, int serial);
int deleteBook(BookStore* store, int serial);
void handleDeleteBook(BookStore* store);
void handleAddBook(BookStore* store);
void handleUpdateBook(BookStore* store);


// Functions for searching books
void searchByText(BookStore* store);           
void searchByNumber(BookStore* store);         
void searchByBestseller(BookStore* store);     
void searchByDate(BookStore* store);           
void handleSearchBooks(BookStore* store);


// Functions for menu and user 
int handleUserInput(int* choice, BookStore* store);
void displayMenu(BookStore* store);

// Function to save the book file
void saveToFile(BookStore* store);


//Functions for Customer management
void initCustomerSystem(BookStore* store);
void loadCustomersFromFile(BookStore* store);
void displayCustomers(BookStore* store);
void handleAddCustomer(BookStore* store);
Customer* findCustomerById(BookStore* store, int id);
void findCustomersByName(BookStore* store);
void handleDeleteCustomer(BookStore* store);
void saveCustomersToFile(BookStore* store);
void handleCustomerManagement(BookStore* store);


//Function to free resources and close files
void cleanup(BookStore* store);

#endif



