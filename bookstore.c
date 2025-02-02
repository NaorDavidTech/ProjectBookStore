//Naor David 

#include "bookstore.h"




// Initializes the bookstore system, sets up file paths, and opens the log file.

void initSystem(BookStore* store, const char* users_path, const char* items_path, const char* customer_path, const char* log_path)
{
    if (!store)
    {
        printf("Error: Invalid store reference\n");
        return;
    }

    store->head = NULL;
    store->tail = NULL;
    store->current = NULL;
    store->currentUserLevel = -1;
    strcpy(store->currentUsername, "");
    store->customerHead = NULL;

    store->usersPath = _strdup(users_path);
    store->itemsPath = _strdup(items_path);
    store->customerPath = _strdup(customer_path);
    store->logPath = _strdup(log_path);

    store->logFile = fopen(store->logPath, "a");
    if (store->logFile == NULL)
    {
        printf("Error opening log file!\n");
        return;
    }

    initCustomerSystem(store);
    
    if (!checkAndCreateFiles(store))
    {
        fprintf(store->logFile, "Error: Failed to initialize system files\n");
        printf("Error initializing system files!\n");
        return;
    }

    fprintf(store->logFile, "System initialized successfully\n");
}






// Creates and initializes the users file with an admin user.

void initUsersFile(BookStore* store) 
{
    if (!store)
    {
        printf("Error: Invalid store reference\n");
        return;
    }

    FILE* fp = fopen(store->usersPath, "w");
    if (!fp) 
    {
        printf("Error creating initial users file!\n");
        fprintf(store->logFile, "Error creating initial users file\n");
        return;
    }

    fprintf(fp, "%-30s %-15s %-10s %-30s\n", "username", "password", "Level", "Full name"); 
    fprintf(fp, "%-30s %-15s %d %-30s\n", "admin", "admin", PERMISSION_MANAGER, "Manager_System");
        
    fclose(fp);
    fprintf(store->logFile, "Created initial users file with admin user\n");
}









// Checks if the users and items files exist, and creates them if necessary.

int checkAndCreateFiles(BookStore* store)
{
    if (!store)
    {
        printf("Error: Invalid store reference\n");
        return 0;  
    }

    FILE* fp;
    fp = fopen(store->usersPath, "r");
    if (!fp)
    {
        initUsersFile(store);
    }
    else
    {
        fclose(fp);
    }

    fp = fopen(store->itemsPath, "r");
    if (!fp)
    {
        fp = fopen(store->itemsPath, "w");
        if (!fp)
        {
            printf("Error creating items file!\n");
            fprintf(store->logFile, "Error creating items file\n");
            return 0;
        }
        fprintf(fp, "%-5s %-60s %-40s %-15s %-10s %-13s %-10s\n", "ID", "Title", "Author", "Genre", "Price", "Bestseller", "Date");
        fprintf(fp, "------------------------------------------------------------------------------------------------------");
        fprintf(fp, "-------------------------------------------------------------\n");
        fclose(fp);
    }
    else
    {
        fclose(fp);
    }
    return 1;
}
















// Retrieves the current date and time, storing the values in the provided pointers.

void getDateTime(int* day, int* month, int* year, int* hours, int* mins)
{
    time_t rawtime;
    struct tm timeinfo;

    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);

    *day = timeinfo.tm_mday;
    *month = timeinfo.tm_mon + 1;
    *year = timeinfo.tm_year + 1900;
    *hours = timeinfo.tm_hour;
    *mins = timeinfo.tm_min;
}









// Validates the provided username and password by comparing them to the user records in the file.
// If valid, stores the user permission level and returns 1. Otherwise, returns 0.

int validateUserCredentials(const char* username, const char* password, int* level, BookStore* store)
{
    if (!store)
    {
        printf("Error: Invalid store reference\n");
        return 0;
    }

    char temp[100];
    char fileUsername[MAX_USERNAME] = { 0 };
    char filePassword[MAX_PASSWORD] = { 0 };
    char fileFullname[MAX_FULLNAME] = { 0 };
    int fileLevel;

    FILE* fp = fopen(store->usersPath, "r");
    if (!fp)
    {
        printf("Error: Cannot access users file!\n");
        fprintf(store->logFile, "Error: Cannot access users file!\n");
        return 0;
    }

    fgets(temp, 100, fp);

    while (fscanf(fp, "%29s %14s %d %29[^\n]", fileUsername, filePassword, &fileLevel, fileFullname) == 4)
    {

        fileUsername[MAX_USERNAME - 1] = '\0';
        filePassword[MAX_PASSWORD - 1] = '\0';
        fileFullname[MAX_FULLNAME - 1] = '\0';

        if (strcmp(username, fileUsername) == 0 && strcmp(password, filePassword) == 0)
        {
            *level = fileLevel;
            fclose(fp);
            return 1;  
        }
    }

    fclose(fp);
    return 0;  
}










// Handles the user login process. Prompts for username and password, validates credentials,
// and allows up to LOGIN_ATTEMPTS attempts. Updates the user's access level if successful.

int handleLogin(BookStore* store)
{
    if (!store)
    {
        printf("Error: System not initialized!\n");
        return 0;
    }

    int attempts = 0;
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int level;
    int day, month, year, hours, mins;
    getDateTime(&day, &month, &year, &hours, &mins);

    printf( "\n=== %02d/%02d/%d, %02d:%02d ===\n", day, month, year, hours, mins);
    fprintf(store->logFile, "\n=== Login Session Started at %02d/%02d/%d, %02d:%02d ===\n", day, month, year, hours, mins);

    while (attempts < LOGIN_ATTEMPTS)
    {
        printf("\nLogin Required\n");
        fprintf(store->logFile, "\nLogin Required\n");
        fprintf(store->logFile, "Login attempt %d of %d\n", attempts + 1, LOGIN_ATTEMPTS);

        printf("Username: ");
        scanf("%29s", username);
        fprintf(store->logFile, "Username: %s\n", username);

        printf("Password: ");
        scanf("%14s", password);
        fprintf(store->logFile, "Password: %s\n", password);

        if (validateUserCredentials(username, password, &level, store))
        {
            store->currentUserLevel = level;
            strcpy(store->currentUsername, username);

            getDateTime(&day, &month, &year, &hours, &mins);

            printf("\nLogin successful! Welcome %s\n", username);
            fprintf(store->logFile, "Login successful at %02d/%02d/%d, %02d:%02d\n", day, month, year, hours, mins);
            fprintf(store->logFile, "User: %s (Permission Level: %d)\n", username, level);

            return 1;
        }

        attempts++;
        printf("\nInvalid username or password!");
        fprintf(store->logFile, "Login failed - Invalid username or password!\n");

        if (attempts < LOGIN_ATTEMPTS)
        {
            printf("\nAttempts remaining: %d\n", LOGIN_ATTEMPTS - attempts);
            fprintf(store->logFile, "Attempts remaining: %d\n", LOGIN_ATTEMPTS - attempts);
        }
    }
    getDateTime(&day, &month, &year, &hours, &mins);

    printf("\nToo many failed attempts. Program will exit.\n");
    fprintf(store->logFile, "\nToo many failed attempts. Program will exit.\n");

    fprintf(store->logFile, "Login blocked at %02d/%02d/%d, %02d:%02d\n", day, month, year, hours, mins);
    fprintf(store->logFile, "Maximum attempts (%d) reached\n", LOGIN_ATTEMPTS);
    fprintf(store->logFile, "=== Login Session Ended ===\n");

    return 0;
}










// Displays the list of users from the users file. Accessible only by admins (PERMISSION_MANAGER).
// The users' details are printed to both the console and the log file.

void displayUsers(BookStore* store)
{
    if (!store)
    {
        printf("Error: Invalid store reference\n");
        return;
    }

    if (store->currentUserLevel != PERMISSION_MANAGER)
    {
        printf("Access denied. Admin permission required.\n");
        fprintf(store->logFile, "Access denied. Admin permission required.\n");
        return;
    }

    char header[100];
    char username[MAX_USERNAME] = { 0 };
    char password[MAX_PASSWORD] = { 0 };
    char fullname[MAX_FULLNAME] = { 0 };
    int level;


    FILE* fp = fopen(store->usersPath, "r");
    if (!fp)
    {
        printf("Error opening users file!\n");
        fprintf(store->logFile, "Error opening users file!\n");
        return;
    }


    fgets(header, sizeof(header), fp);

    printf("\n=== Users List ===\n");
    printf("\n");
    fprintf(store->logFile, "\n=== Users List ===\n");
    fprintf(store->logFile, "\n");
    
    
    printf("%-30s %-15s %-10s %-30s\n", "Username", "Password", "Level", "Full Name");    
    printf("---------------------------------------------------------------------------------\n");
    fprintf(store->logFile,"%-30s %-15s %-10s %-30s\n", "Username", "Password", "Level", "Full Name");
    fprintf(store->logFile,"---------------------------------------------------------------------------------\n");
    
    
    
    while (fscanf(fp, "%29s %14s %d %29[^\n]", username, password, &level, fullname) == 4)  
    {
        username[MAX_USERNAME - 1] = '\0';
        password[MAX_PASSWORD - 1] = '\0';
        fullname[MAX_FULLNAME - 1] = '\0';
        printf("%-30s %-15s %-10d %-30s\n", username, password, level, fullname);  
        fprintf(store->logFile, "%-30s %-15s %-10d %-30s\n", username, password, level, fullname);
  
     
    }
    fclose(fp);
    printf("\n");
}







// This function allows the manager to add a new user to the system. It first checks if the current user has the necessary permissions (Manager level).
// Then, it prompts the manager to enter the new user's details, including username, password, full name, and user level.
// The function checks if the username already exists to avoid duplicates. If the user level is invalid, it defaults to "View Only".

void handleAddUser(BookStore* store)
{
    if (!store)
    {
        printf("Error: Invalid store reference\n");
        return;
    }

    if (store->currentUserLevel != PERMISSION_MANAGER)
    {
        printf("Access denied. MANAGER permissions required.\n");
        fprintf(store->logFile, "Unauthorized user addition attempt by %s\n", store->currentUsername);
        return;
    }


    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    char fullname[MAX_FULLNAME];
    int level;
    char tempUsername[MAX_USERNAME];
    char tempPassword[MAX_PASSWORD];
    char tempFullname[MAX_FULLNAME];
    int tempLevel;
    int userExists = 0;

    printf("\n=== Add New User ===\n");
    fprintf(store->logFile, "\n=== Add New User ===\n");

    printf("Enter username (max %d characters): ", MAX_USERNAME - 1);
    scanf("%29s", username);
    while (getchar() != '\n');

    fprintf(store->logFile, "Enter username (max %d characters): ", MAX_USERNAME - 1);
    fprintf(store->logFile, "User entered:%14s\n", username);


    FILE* fp = fopen(store->usersPath, "r");
    if (!fp) {
        printf("Error accessing users file!\n");
        fprintf(store->logFile, "Error accessing users file!\n");
        return;
    }


    while (fscanf(fp, "%29s %14s %d %29[^\n]",
        tempUsername, tempPassword, &tempLevel, tempFullname) == 4)
    {
        if (strcmp(username, tempUsername) == 0)
        {
            userExists = 1;
            break;
        }
    }
    fclose(fp);

    if (userExists)
    {
        printf("Error: Username already exists!\n");
        fprintf(store->logFile, "Failed to add user - username %s already exists\n", username);
        return;
    }

    printf("Enter password (max %d characters): ", MAX_PASSWORD - 1);
    scanf("%14s", password);
    while (getchar() != '\n');
    fprintf(store->logFile, "Enter password (max %d characters): ", MAX_PASSWORD - 1);
    fprintf(store->logFile, "User entered:%14s\n", password);


    printf("Enter full name (max %d characters): ", MAX_FULLNAME - 1);
    fgets(fullname, MAX_FULLNAME, stdin);
    fullname[strcspn(fullname, "\n")] = 0;
    fprintf(store->logFile, "Enter full name (max %d characters): ", MAX_FULLNAME - 1);
    fprintf(store->logFile, "User entered:%s\n", fullname);


    printf("Enter user level (1-3):\n");
    printf("1 - View\n");
    printf("2 - Edit\n");
    printf("3 - Manager\n");
    printf("Your choice: ");
    scanf("%d", &level);
    while (getchar() != '\n');

    fprintf(store->logFile, "Enter user level (1-3):\n");
    fprintf(store->logFile, "1 - View \n");
    fprintf(store->logFile, "2 - Edit\n");
    fprintf(store->logFile, "3 - Manager\n");
    fprintf(store->logFile, "Your choice: ");
    fprintf(store->logFile, "%d", level);


    if (level < PERMISSION_ONE || level > PERMISSION_MANAGER)
    {
        printf("Invalid user level. Defaulting to View Only.\n");
        fprintf(store->logFile, "Invalid user level. Defaulting to View Only.\n");
        level = PERMISSION_ONE;
    }

    fp = fopen(store->usersPath, "a");
    if (!fp)
    {
        printf("Error opening users file for writing!\n");
        fprintf(store->logFile, "Error opening users file to add new user\n");
        return;
    }

    fprintf(fp, "%-30s %-15s %d %-30s\n", username, password, level, fullname);
    fclose(fp);

    printf("\nUser added successfully!\n");
    fprintf(store->logFile, "New user added: %s (Level: %d)\n", username, level);
}











int checkUserExists(const char* username, BookStore* store)
{
    if (!store || !username) return 0;

    char temp[100];
    char fileUsername[MAX_USERNAME] = { 0 };
    char filePassword[MAX_PASSWORD] = { 0 };
    char fileFullname[MAX_FULLNAME] = { 0 };
    int fileLevel;

    FILE* fp = fopen(store->usersPath, "r");
    if (!fp)
    {
        printf("Error: Cannot access users file!\n");
        fprintf(store->logFile, "Error: Cannot access users file!\n");
        return 0;
    }

    
    fgets(temp, 100, fp);

    while (fscanf(fp, "%29s %14s %d %29[^\n]",
        fileUsername, filePassword, &fileLevel, fileFullname) == 4)
    {
        fileUsername[MAX_USERNAME - 1] = '\0';
        if (strcmp(username, fileUsername) == 0)
        {
            fclose(fp);
            return 1;  
        }
    }

    fclose(fp);
    return 0; 
}













void handleDeleteUser(BookStore* store)
{
    if (!store) 
    {
        printf("Error: Invalid store reference\n");
        return;
    }

    if (store->currentUserLevel != PERMISSION_MANAGER)
    {
        printf("Access denied. MANAGER permissions required.\n");
        fprintf(store->logFile, "Unauthorized user deletion attempt by %s\n", store->currentUsername);
        return;
    }

    char username[MAX_USERNAME] = { 0 };
    char confirm;
   

    printf("\n=== Delete User ===\n");
    fprintf(store->logFile, "\n=== Delete User ===\n");

    printf("Enter username to delete: ");
    if (scanf("%29s", username) != 1)
    {
        printf("Error reading username\n");
        fprintf(store->logFile, "Error reading username input\n");
        return;
    }
    username[MAX_USERNAME - 1] = '\0';
    while (getchar() != '\n');

    fprintf(store->logFile, "Delete user attempt for username: %s\n", username);

    if (strcmp(username, "admin") == 0)
    {
        printf("Error: Cannot delete admin account!\n");
        fprintf(store->logFile, "Attempt to delete admin account denied\n");
        return;
    }

    if (!checkUserExists(username, store))
    {
        printf("User not found!\n");
        fprintf(store->logFile, "Delete user failed: User %s not found\n", username);
        return;
    }

    printf("Are you sure you want to delete user %s? (y/n): ", username);
    if (scanf(" %c", &confirm) != 1) 
    {
        printf("Error reading confirmation\n");
        fprintf(store->logFile, "Error reading delete confirmation\n");
        return;
    }
    while (getchar() != '\n');

    if (confirm == 'y' || confirm == 'Y')
    {
        FILE* fp = fopen(store->usersPath, "r");
        FILE* tempFile = fopen("temp_users.txt", "w");

        if (!fp || !tempFile)
        {
            printf("Error accessing files!\n");
            fprintf(store->logFile, "Error accessing files for user deletion\n");
            if (fp) fclose(fp);
            if (tempFile) fclose(tempFile);
            return;
        }

        char line[256] = { 0 };
        char tempUsername[MAX_USERNAME] = { 0 };

        
        if (fgets(line, sizeof(line), fp)) 
        {
            fprintf(tempFile, "%s", line);
        }

 
        while (fgets(line, sizeof(line), fp))
        {
            if (sscanf(line, "%29s", tempUsername) == 1)
            {
                if (strcmp(tempUsername, username) != 0)
                {
                    fprintf(tempFile, "%s", line);
                }
            }
        }

        fclose(fp);
        fclose(tempFile);

     
        fp = fopen(store->usersPath, "w");
        if (!fp)
        {
            printf("Error updating users file!\n");
            fprintf(store->logFile, "Error updating users file\n");
            return;
        }

     
        tempFile = fopen("temp_users.txt", "r");
        if (!tempFile)
        {
            printf("Error reading temporary file!\n");
            fprintf(store->logFile, "Error reading temporary file\n");
            fclose(fp);
            return;
        }

        while (fgets(line, sizeof(line), tempFile))
        {
            fprintf(fp, "%s", line);
        }

        fclose(fp);
        fclose(tempFile);

        printf("User deleted successfully!\n");
        fprintf(store->logFile, "User %s deleted successfully\n", username);
    }
    else
    {
        printf("Delete operation cancelled\n");
        fprintf(store->logFile, "User deletion cancelled for %s\n", username);
    }
}















// Creates a new book object with the given details (serial number, title, author, price, bestseller status, and publish date).
// Validates input values (date, price, serial number, title, and author length) before allocating memory for the book object.
// If any validation fails, it logs an error and returns NULL.

Book* createBook(int serial, const char* title, const char* author, const char* genre, double price, bool is_bestseller, int day, int month, int year, BookStore* store)
{

    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900) 
    {
        printf("Error: Invalid date values\n");
        fprintf(store->logFile, "Error: Invalid date values\n");
        return NULL;
    }

    if (price < 0 || serial < 0) 
    {
        printf("Error: Price and serial number must be positive\n");
        fprintf(store->logFile,"Error: Price and serial number must be positive\n");
        return NULL;
    }

    if (is_bestseller != true && is_bestseller != false) 
    {
        printf("Error: is_bestseller must be TRUE or FALSE\n");
        fprintf(store->logFile,"Error: is_bestseller must be TRUE or FALSE\n");
        return NULL;
    }

    Book* newBook = (Book*)malloc(sizeof(Book));
    if (newBook == NULL) 
    {
        printf("Error: Memory allocation failed\n");
        fprintf(store->logFile, "Error: Memory allocation failed\n");
        return NULL;
    }

    if (strlen(title) >= MAX_TITLE || strlen(author) >= MAX_AUTHOR || strlen(genre) >= MAX_GENRE)
    {
        printf("Error: Title or author name too long\n");
        fprintf(store->logFile,"Error: Title or author name too long\n");
        free(newBook);
        return NULL;
    }

    newBook->serial_number = serial;
    strcpy(newBook->title, title);
    strcpy(newBook->author, author);
    strcpy(newBook->genre, genre);
    newBook->price = price;
    newBook->is_bestseller = is_bestseller;
    newBook->publish_date.day = day;
    newBook->publish_date.month = month;
    newBook->publish_date.year = year;
    newBook->next = NULL;
    newBook->prev = NULL;

    return newBook;
}






// Inserts a new book into the store's linked list of books, maintaining the list sorted by serial number.
// If the list is empty, the new book becomes both the head and tail. 
// If the new book's serial number is smaller than the head's, it becomes the new head. 
// Otherwise, it finds the correct position in the list and inserts the book while preserving the order.

void insertBook(BookStore* store, Book* newBook) 
{
    if (!store || !newBook) 
    {
        printf("Error: Invalid parameters for insertion\n");
        return;
    }

    if (!store->head) 
    {
        store->head = store->tail = newBook;
        newBook->next = newBook->prev = NULL;
        return;
    }

    Book* current = store->head;

    if (newBook->serial_number < current->serial_number) 
    {
        newBook->next = store->head;
        newBook->prev = NULL;
        store->head->prev = newBook;
        store->head = newBook;
        return;
    }

    while (current->next && current->next->serial_number < newBook->serial_number)  
    {
        current = current->next;
    }

    if (!current->next) 
    {
        current->next = newBook;
        newBook->prev = current;
        newBook->next = NULL;
        store->tail = newBook;
    }

    else 
    {
        newBook->next = current->next;
        newBook->prev = current;
        current->next->prev = newBook;
        current->next = newBook;
    }
}











// Loads books from the items file and inserts them into the store's linked list.


void loadBooksFromFile(BookStore* store)
{
    FILE* fp = fopen(store->itemsPath, "r");
    if (!fp)
    {
        printf("Error opening items file: %s\n", store->itemsPath);
        fprintf(store->logFile, "Error opening items file\n");
        return;
    }

    char header[300];
    fgets(header, sizeof(header), fp);
    fgets(header, sizeof(header), fp);
    char line[500];
    int booksLoaded = 0;
    Book** tempBooks = NULL;
    int capacity = 20;
    int count = 0;


    tempBooks = (Book**)malloc(capacity * sizeof(Book*));
    if (!tempBooks)
    {
        printf("Memory allocation failed\n");
        fprintf(store->logFile, "Memory allocation failed\n");
        fclose(fp);
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        int serial_number;
        char title[MAX_TITLE] = { 0 };
        char author[MAX_AUTHOR] = { 0 };
        char genre[MAX_GENRE] = { 0 };
        double price;
        char bestseller[20] = { 0 };
        int day, month, year;


        if (sscanf(line, "%d %59[^\t] %39[^\t] %14[^\t] %lf %19s %d/%d/%d",
            &serial_number, title, author, genre, &price, bestseller,
            &day, &month, &year) == 9)
        {
            if (count >= capacity)
            {
                capacity *= 2;

                Book** temp = (Book**)realloc(tempBooks, capacity * sizeof(Book*));
                if (!temp)
                {
                    printf("Memory reallocation failed\n");
                    fprintf(store->logFile, "Memory reallocation failed\n");
                    break;
                }
                tempBooks = temp;
            }

            bool is_best = (strcmp(bestseller, "Yes") == 0) ? true : false;
            Book* newBook = createBook(serial_number, title, author, genre, price, is_best, day, month, year, store);
            if (newBook)
            {
                tempBooks[count++] = newBook;
            }
        }
    }

    fclose(fp);
    for (int i = 0; i < count; i++)
    {
        insertBook(store, tempBooks[i]);
        booksLoaded++;
    }
    free(tempBooks);
    fprintf(store->logFile, "Total books loaded: %d\n", booksLoaded);
}










// Displays the list of books in the store, including details like ID, title, author, price, bestseller status, and publish date.
// The books are printed to both the console and the log file. The total number of books is also displayed.

void displayBooks(BookStore* store)
{
    if (!store)
    {
        printf("Error: Invalid store reference\n");
        return;
    }

    Book* current = store->head;
    int bookCount = 0;

    printf("\n=== Books List ===\n");
    printf("\n");

    fprintf(store->logFile, "Displaying books list\n");
    fprintf(store->logFile, "\n");

    printf("%-5s %-60s %-40s %-15s %-10s %-13s %-10s\n", "ID", "Title", "Author", "Genre", "Price", "Bestseller", "Date");
    printf("------------------------------------------------------------------------------------------------------");
    printf("-------------------------------------------------------------\n");

    fprintf(store->logFile, "%-5s %-60s %-40s %-15s %-10s %-13s %-10s\n", "ID", "Title", "Author", "Genre", "Price", "Bestseller", "Date");
    fprintf(store->logFile, "------------------------------------------------------------------------------------------------------");
    fprintf(store->logFile, "-------------------------------------------------------------\n");


    while (current != NULL)
    {
        printf("%-5d %-60s %-40s %-15s %-10.2f %-13s %02d/%02d/%04d\n",
            current->serial_number,
            current->title,
            current->author,
            current->genre,
            current->price,
            current->is_bestseller ? "Yes" : "No",
            current->publish_date.day,
            current->publish_date.month,
            current->publish_date.year);

        fprintf(store->logFile, "%-5d %-60s %-40s %-15s %-10.2f %-13s %02d/%02d/%04d\n",
            current->serial_number,
            current->title,
            current->author,
            current->genre,
            current->price,
            current->is_bestseller ? "Yes" : "No",
            current->publish_date.day,
            current->publish_date.month,
            current->publish_date.year);


        bookCount++;
        current = current->next;
    }

    printf("\nTotal books: %d\n", bookCount);
    fprintf(store->logFile, "Total books: %d\n", bookCount);
}













// Searches for a book in the store's linked list by its serial number.
// If the book is found, it prints and logs the book's title and serial number, and returns a pointer to the book.

Book* findBook(BookStore* store, int serial) 
{
    if (!store) 
    {
        printf("Error: Invalid store reference\n");
        return NULL;
    }

    if (!store->head) 
    {
        printf("No books in store!\n");
        fprintf(store->logFile, "No books in store!\n");
        return NULL;
    }

    Book* current = store->head;
    while (current) 
    {
        if (current->serial_number == serial) 
        {
            printf("Book found: %s \n",current->title);
            fprintf(store->logFile, "Book found: %s \n", current->title);
                
            return current;
        }
        current = current->next;
    }

    printf("Book not found: Serial %d\n", serial);
    fprintf(store->logFile, "Book not found: Serial %d\n", serial);
    return NULL;
}









// Deletes a book from the store's linked list by its serial number.
// If the book is found, it updates the linked list to remove the book and frees its memory.
// The function returns 1 if the book is successfully deleted, or 0 if the book is not found.

int deleteBook(BookStore* store, int serial)
{
    if (!store)
    {
        printf("Error: Invalid store reference\n");
        return 0;
    }

    Book* bookToDelete = findBook(store, serial);
    if (!bookToDelete) 
    {
        printf("Delete failed: Book not found (Serial: %d)\n", serial);
        fprintf(store->logFile, "Delete failed: Book not found (Serial: %d)\n", serial);
        return 0;
    }

    if (store->current == bookToDelete)
    {
        if (bookToDelete->next) 
        {
            store->current = bookToDelete->next;
        }

        else if (bookToDelete->prev) 
        {
            store->current = bookToDelete->prev;
        }

        else 
        {
            store->current = NULL;  
        }
        
    }

    if (bookToDelete == store->head)
    {
        store->head = bookToDelete->next;
        if (store->head) 
        {
            store->head->prev = NULL;
        }
        
    }
    
    else if (bookToDelete == store->tail)
    {
        store->tail = bookToDelete->prev;
        if (store->tail) 
        {
            store->tail->next = NULL;
        }
        
    }
    
    else
    {
        if (bookToDelete->prev) 
        {
            bookToDelete->prev->next = bookToDelete->next;
        }

        if (bookToDelete->next) 
        {
            bookToDelete->next->prev = bookToDelete->prev;
        }
        
    }

    free(bookToDelete);

    return 1;  
}








// The function allows the user to delete a book from the store by entering its serial number.
// It first checks if the book exists, displays its details for confirmation, and prompts the user for confirmation.
// If the user confirms, the book is deleted from the store, and the updated list is written back to the file.

void handleDeleteBook(BookStore* store)
{
    if (!store) return;

    int serial;
    char confirm;

    printf("\n=== Delete Book ===\n");
    fprintf(store->logFile, "\n=== Delete Book ===\n");

    printf("Enter serial number of book to delete: ");
    fprintf(store->logFile, "Enter serial number of book to delete: ");

    if (scanf("%d", &serial) != 1 )
    {
        printf("Error: Serial number must be a positive number!\n");
        fprintf(store->logFile, "Error: Invalid serial number input\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    fprintf(store->logFile, " User entered:%d\n", serial);

    Book* bookToDelete = findBook(store, serial);
    if (!bookToDelete)
    {
        printf("Book not found!\n");
        fprintf(store->logFile, "Book not found!\n");
        return;
    }


    printf("\nBook to delete:\n");
    printf("Title: %s\n", bookToDelete->title);
    printf("Author: %s\n", bookToDelete->author);
    printf("Genre: %s\n", bookToDelete->genre);
    printf("Price: %.2f\n", bookToDelete->price);
    printf("Bestseller: %s\n", bookToDelete->is_bestseller ? "Yes" : "No");
    printf("Publication date: %02d/%02d/%04d\n",
        bookToDelete->publish_date.day,
        bookToDelete->publish_date.month,
        bookToDelete->publish_date.year);



    fprintf(store->logFile, "\nBook to delete:\n");
    fprintf(store->logFile, "Title: %s\n", bookToDelete->title);
    fprintf(store->logFile, "Author: %s\n", bookToDelete->author);
    fprintf(store->logFile, "Genre: %s\n", bookToDelete->genre);
    fprintf(store->logFile, "Price: %.2f\n", bookToDelete->price);
    fprintf(store->logFile, "Bestseller: %s\n", bookToDelete->is_bestseller ? "Yes" : "No");
    fprintf(store->logFile, "Publication date: %02d/%02d/%04d\n",
        bookToDelete->publish_date.day,
        bookToDelete->publish_date.month,
        bookToDelete->publish_date.year);



    printf("\nAre you sure you want to delete this book? (y/n): ");
    fprintf(store->logFile, "\nAre you sure you want to delete this book? (y/n): ");

    scanf(" %c", &confirm);
    while (getchar() != '\n');

    fprintf(store->logFile, "User entered: %c\n", confirm);

    if (confirm == 'y' || confirm == 'Y')
    {
        deleteBook(store, serial);
        printf("Book deleted successfully!\n");
        fprintf(store->logFile, "Book deleted successfully!\n");

    }
    else
    {
        printf("Delete operation cancelled\n");
        fprintf(store->logFile, "Delete operation cancelled\n");
    }
}










// The function allows the user to add a new book to the store.
// It collects information about the book ,checks if a book with the same serial number already exists, and adds the book if valid.

void handleAddBook(BookStore* store)
{
    if (!store) return;

    int temp;
    Book newBook;

    printf("\n=== Add New Book ===\n");
    fprintf(store->logFile, "\n=== Add New Book ===\n");

    printf("Enter serial number: ");

    if (scanf("%d", &newBook.serial_number) != 1 )
    {
        printf("Error: Serial number must be a positive number!\n");
        fprintf(store->logFile, "Error: Invalid serial number input\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    fprintf(store->logFile, "Enter serial number: ");
    fprintf(store->logFile, "User entered:%d\n", newBook.serial_number);

    if (findBook(store, newBook.serial_number) != NULL)
    {
        printf("Error: Book with this serial number already exists!\n");
        fprintf(store->logFile, "Error: Book with this serial number already exists!%d\n", newBook.serial_number);
        return;
    }

    printf("Enter title: ");
    fgets(newBook.title, MAX_TITLE, stdin);
    newBook.title[strcspn(newBook.title, "\n")] = 0;
    fprintf(store->logFile, "Enter title: ");
    fprintf(store->logFile, "User entered : %s\n", newBook.title);

    printf("Enter author: ");
    fgets(newBook.author, MAX_AUTHOR, stdin);
    newBook.author[strcspn(newBook.author, "\n")] = 0;
    fprintf(store->logFile, "Enter author: ");
    fprintf(store->logFile, "User entered : %s\n", newBook.author);


    printf("Enter genre: ");
    fgets(newBook.genre, MAX_GENRE, stdin);
    newBook.genre[strcspn(newBook.genre, "\n")] = 0;
    fprintf(store->logFile, "Enter genre: ");
    fprintf(store->logFile, "User entered : %s\n", newBook.genre);


    printf("Enter price: ");

    if (scanf("%lf", &newBook.price) != 1 || newBook.price <= 0)
    {
        printf("Error: Serial number must be a positive number!\n");
        fprintf(store->logFile, "Error: Invalid serial number input\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    fprintf(store->logFile, "Enter price: ");
    fprintf(store->logFile, "User entered:%lf\n", newBook.price);


    printf("Is bestseller? (1 for Yes, 0 for No): ");
    scanf("%d", &temp);
    newBook.is_bestseller = (temp != 0);
    while (getchar() != '\n');

    fprintf(store->logFile, "Is bestseller? (1 for Yes, 0 for No): ");
    fprintf(store->logFile, "User entered:%d\n", temp);


    printf("Enter publication date (DD MM YYYY): ");
    printf("\n");

    printf("Enter day: ");
    if (scanf("%d", &newBook.publish_date.day) != 1 || newBook.publish_date.day <= 0)
    {
        printf("Error: Input must be a number!\n");
        fprintf(store->logFile, "Error: Invalid day input - not a number\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    printf("Enter month : ");
    if (scanf("%d", &newBook.publish_date.month) != 1 || newBook.publish_date.month <= 0)
    {
        printf("Error: Input must be a number!\n");
        fprintf(store->logFile, "Error: Invalid month input - not a number\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    printf("Enter year: ");
    if (scanf("%d", &newBook.publish_date.year) != 1 || newBook.publish_date.year <= 0)
    {
        printf("Error: Input must be a number!\n");
        fprintf(store->logFile, "Error: Invalid year input - not a number\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    fprintf(store->logFile, "Enter publication date (DD MM YYYY): ");
    fprintf(store->logFile, "\n");
    fprintf(store->logFile, "Enter day:");
    fprintf(store->logFile, "User entered:%d\n", newBook.publish_date.day);
    fprintf(store->logFile, "Enter month:");
    fprintf(store->logFile, "User entered:%d\n", newBook.publish_date.month);
    fprintf(store->logFile, "Enter year:");
    fprintf(store->logFile, "User entered:%d\n", newBook.publish_date.year);



    Book* book = createBook(newBook.serial_number,
        newBook.title,
        newBook.author,
        newBook.genre,
        newBook.price,
        newBook.is_bestseller,
        newBook.publish_date.day,
        newBook.publish_date.month,
        newBook.publish_date.year,
        store);

    if (book != NULL)
    {
        insertBook(store, book);
        printf("\nBook added successfully!\n");
        fprintf(store->logFile, "Book added: %s by %s\n", book->title, book->author);
    }
    else
    {
        printf("\nError adding book!\n");
        while (getchar() != '\n');
        fprintf(store->logFile, "\nError adding book!\n");
    }
}










// The function allows the user to update details of an existing book in the bookstore.
// The user can update fields, or cancel the update.
// It validates the input, displays the current details, and then applies the changes based on the user's choice.

void handleUpdateBook(BookStore* store)
{
    if (!store) return;

    int serial;
    int choice;
    int temp;

    printf("\n=== Update Book ===\n");
    fprintf(store->logFile, "\n=== Update Book ===\n");

    printf("Enter serial number of book to update: ");

    if (scanf("%d", &serial) != 1 )
    {
        printf("Error: Serial number must be a positive number!\n");
        fprintf(store->logFile, "Error: Invalid serial number input\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    fprintf(store->logFile, "Enter serial number of book to update: ");
    fprintf(store->logFile, "User entered:%d\n", serial);


    Book* bookToUpdate = findBook(store, serial);
    if (bookToUpdate == NULL)
    {
        printf("Book not found!\n");
        fprintf(store->logFile, "Update attempt failed: Book with serial %d not found\n", serial);
        return;
    }

    printf("\nCurrent book details:\n");
    printf("1. Title: %s\n", bookToUpdate->title);
    printf("2. Author: %s\n", bookToUpdate->author);
    printf("3. Genre: %s\n", bookToUpdate->genre);
    printf("4. Price: %.2f\n", bookToUpdate->price);
    printf("5. Bestseller status: %s\n", bookToUpdate->is_bestseller ? "Yes" : "No");
    printf("6. Publication date: %02d/%02d/%04d\n",
        bookToUpdate->publish_date.day,
        bookToUpdate->publish_date.month,
        bookToUpdate->publish_date.year);
    printf("7. All fields\n");
    printf("0. Cancel update\n");



    fprintf(store->logFile, "\nCurrent book details:\n");
    fprintf(store->logFile, "1. Title: %s\n", bookToUpdate->title);
    fprintf(store->logFile, "2. Author: %s\n", bookToUpdate->author);
    fprintf(store->logFile, "3. Genre: %s\n", bookToUpdate->genre);
    fprintf(store->logFile, "4. Price: %.2f\n", bookToUpdate->price);
    fprintf(store->logFile, "5. Bestseller status: %s\n", bookToUpdate->is_bestseller ? "Yes" : "No");
    fprintf(store->logFile, "6. Publication date: %02d/%02d/%04d\n",
        bookToUpdate->publish_date.day,
        bookToUpdate->publish_date.month,
        bookToUpdate->publish_date.year);
    fprintf(store->logFile, "7. All fields\n");
    fprintf(store->logFile, "0. Cancel update\n");


    printf("\nEnter field to update (0-7): ");
    scanf("%d", &choice);
    while (getchar() != '\n');
    fprintf(store->logFile, "\nEnter field to update (0-7): ");
    fprintf(store->logFile, "User entered:%d\n", choice);


    switch (choice)
    {
    case 0:
        printf("Update cancelled\n");
        fprintf(store->logFile, "Book update cancelled by user\n");
        return;

    case 1:
        printf("Enter new title: ");
        fgets(bookToUpdate->title, MAX_TITLE, stdin);
        bookToUpdate->title[strcspn(bookToUpdate->title, "\n")] = 0;
        fprintf(store->logFile, "Enter new title: ");
        fprintf(store->logFile, " User entered: %s\n", bookToUpdate->title);
        break;

    case 2:
        printf("Enter new author: ");
        fgets(bookToUpdate->author, MAX_AUTHOR, stdin);
        bookToUpdate->author[strcspn(bookToUpdate->author, "\n")] = 0;
        fprintf(store->logFile, "Enter new author: ");
        fprintf(store->logFile, " User entered: %s\n", bookToUpdate->author);
        break;

    case 3:
        printf("Enter new genre: ");
        fgets(bookToUpdate->genre, MAX_GENRE, stdin);
        bookToUpdate->genre[strcspn(bookToUpdate->genre, "\n")] = 0;
        fprintf(store->logFile, "Enter new genre: ");
        fprintf(store->logFile, " User entered: %s\n", bookToUpdate->genre);
        break;
    case 4:
        printf("Enter new price: ");
        scanf("%lf", &bookToUpdate->price);
        fprintf(store->logFile, "Enter new price: ");
        fprintf(store->logFile, "User entered:%lf\n", bookToUpdate->price);
        break;

    case 5:
        printf("Enter new bestseller status (1 for Yes, 0 for No): ");
        scanf("%d", &temp);
        bookToUpdate->is_bestseller = (temp != 0);
        fprintf(store->logFile, "Enter new bestseller status (1 for Yes, 0 for No): ");
        fprintf(store->logFile, " User entered: %d\n", temp);
        break;

    case 6:
        printf("Enter new publication date (DD MM YYYY): ");
        printf("\n");
        printf("Enter day: ");
        scanf("%d", &bookToUpdate->publish_date.day);
        printf("Enter month : ");
        scanf("%d", &bookToUpdate->publish_date.month);
        printf("Enter year: ");
        scanf("%d", &bookToUpdate->publish_date.year);
        fprintf(store->logFile, "Enter new publication date (DD MM YYYY): \n");
        fprintf(store->logFile, "Enter day: ");
        fprintf(store->logFile, " User entered:  %d", bookToUpdate->publish_date.day);
        fprintf(store->logFile, "Enter month : ");
        fprintf(store->logFile, " User entered:  %d", bookToUpdate->publish_date.month);
        fprintf(store->logFile, "Enter year: ");
        fprintf(store->logFile, " User entered:  %d", bookToUpdate->publish_date.year);
        break;

    case 7:
        printf("Enter new title: ");
        fgets(bookToUpdate->title, MAX_TITLE, stdin);
        bookToUpdate->title[strcspn(bookToUpdate->title, "\n")] = 0;
        fprintf(store->logFile, "Enter new title: ");
        fprintf(store->logFile, " User entered: %s\n", bookToUpdate->title);
        printf("Enter new author: ");
        fgets(bookToUpdate->author, MAX_AUTHOR, stdin);
        bookToUpdate->author[strcspn(bookToUpdate->author, "\n")] = 0;
        fprintf(store->logFile, "Enter new author: ");
        fprintf(store->logFile, " User entered: %s\n", bookToUpdate->author);
        printf("Enter new genre: ");
        fgets(bookToUpdate->genre, MAX_GENRE, stdin);
        bookToUpdate->genre[strcspn(bookToUpdate->genre, "\n")] = 0;
        fprintf(store->logFile, "Enter new genre: ");
        fprintf(store->logFile, " User entered: %s\n", bookToUpdate->genre);
        printf("Enter new price: ");
        scanf("%lf", &bookToUpdate->price);
        fprintf(store->logFile, "Enter new price: ");
        fprintf(store->logFile, "User entered:%lf\n", bookToUpdate->price);
        printf("Enter new bestseller status (1 for Yes, 0 for No): ");
        scanf("%d", &temp);
        bookToUpdate->is_bestseller = (temp != 0);
        fprintf(store->logFile, "Enter new bestseller status (1 for Yes, 0 for No): ");
        fprintf(store->logFile, " User entered: %d\n", temp);
        printf("Enter new publication date (DD MM YYYY): ");
        printf("\n");
        printf("Enter day: ");
        scanf("%d", &bookToUpdate->publish_date.day);
        printf("Enter month : ");
        scanf("%d", &bookToUpdate->publish_date.month);
        printf("Enter year: ");
        scanf("%d", &bookToUpdate->publish_date.year);
        fprintf(store->logFile, "Enter new publication date (DD MM YYYY): \n");
        fprintf(store->logFile, "Enter day: ");
        fprintf(store->logFile, " User entered:  %d", bookToUpdate->publish_date.day);
        fprintf(store->logFile, "Enter month : ");
        fprintf(store->logFile, " User entered:  %d", bookToUpdate->publish_date.month);
        fprintf(store->logFile, "Enter year: ");
        fprintf(store->logFile, " User entered:  %d", bookToUpdate->publish_date.year);
        break;

    default:
        printf("Error: Invalid choice! \n");
        fprintf(store->logFile, "Invalid update choice selected\n");
        return;
    }

    printf("\nBook updated successfully!\n");
    fprintf(store->logFile, "Book updated: %s (Serial: %d) - Field %d changed\n",
        bookToUpdate->title, serial, choice);
}












//The function allows the user to search for books by title, author, or both. 
//It displays the results with the book details and allows updating books if the user has the appropriate permissions. 

void searchByText(BookStore* store) 
{
    if (!store || !store->head) 
    {
        printf("No books available to search\n");
        fprintf(store->logFile, "No books available to search\n");
        return;
    }

    int searchChoice;
    char searchTitle[MAX_TITLE] = { 0 };
    char searchAuthor[MAX_AUTHOR] = { 0 };
    int found = 0;
    Book* matchingBooks[100];
    int maxBooks = 100;
    int choice;

    printf("\nSearch by text fields:\n");
    printf("1. Title only\n");
    printf("2. Author only\n");
    printf("3. Both title and author\n");
    printf("0. Return to main menu\n\n");

    fprintf(store->logFile,"\nSearch by text fields:\n");
    fprintf(store->logFile,"1. Title only\n");
    fprintf(store->logFile,"2. Author only\n");
    fprintf(store->logFile,"3. Both title and author\n");
    fprintf(store->logFile,"0. Return to main menu\n\n");
    
    printf("Enter your choice: ");
    scanf("%d", &searchChoice);
    while (getchar() != '\n');

    if (searchChoice == 0)
    {
        return;
    }

    if (searchChoice != 1 && searchChoice != 2 && searchChoice != 3)
    {
        printf("Error: Invalid choice! \n");
        fprintf(store->logFile, "Error: Invalid choice entered: %d\n", searchChoice);
        return;
    }
    
    fprintf(store->logFile,"Enter your choice (1-3): ");
    fprintf(store->logFile, "User entered:%d\n", searchChoice);


    if (searchChoice == 1 || searchChoice == 3) 
    {
        printf("Enter title to search: ");
        fprintf(store->logFile,"Enter title to search: ");
        fgets(searchTitle, MAX_TITLE, stdin);
        searchTitle[strcspn(searchTitle, "\n")] = 0;
        fprintf(store->logFile, "User entered: %s\n", searchTitle);
    }

    if (searchChoice == 2 || searchChoice == 3) 
    {
        printf("Enter author to search: ");
        fprintf(store->logFile,"Enter author to search: ");
        fgets(searchAuthor, MAX_AUTHOR, stdin);
        searchAuthor[strcspn(searchAuthor, "\n")] = 0;
        fprintf(store->logFile, "User entered: %s\n", searchAuthor);
    }

    printf("\nSearch Results:\n");
    printf("%-5s %-60s %-40s %-15s %-10s %-13s %-10s\n", "ID", "Title", "Author", "Genre", "Price", "Bestseller", "Date");
    printf("------------------------------------------------------------------------------------------------------");
    printf("-------------------------------------------------------------\n");

    
    fprintf(store->logFile, "\nSearch Results:\n");
    fprintf(store->logFile, "%-5s %-60s %-40s% -15s %-10s %-13s %-10s\n", "ID", "Title", "Genre", "Author", "Price", "Bestseller", "Date");
    fprintf(store->logFile, "------------------------------------------------------------------------------------------------------");
    fprintf(store->logFile, "-------------------------------------------------------------\n");

    
    Book* current = store->head;
    while (current) 
    {
        int titleMatch = (searchChoice == 2) ||
            (strlen(searchTitle) == 0) ||
            (strstr(current->title, searchTitle) != NULL);

        int authorMatch = (searchChoice == 1) ||
            (strlen(searchAuthor) == 0) ||
            (strstr(current->author, searchAuthor) != NULL);

        if (titleMatch && authorMatch) 
        {
            char date[11];
            sprintf(date, "%02d/%02d/%04d",
                current->publish_date.day,
                current->publish_date.month,
                current->publish_date.year);

            printf("%-5d %-60s %-40s %-15s %-10.2f ",
                current->serial_number,
                current->title,
                current->author,
                current->genre,
                current->price);


            fprintf(store->logFile,"%-5d %-60s %-40s %-15s %-10.2f ",
                current->serial_number,
                current->title,
                current->author,
                current->genre,
                current->price);

            
            if (current->is_bestseller == true) 
            {
                printf("%-13s ", "Yes");
                fprintf(store->logFile, "%-13s ", "Yes");
            }

            else 
            {
                printf("%-13s ", "No");
                fprintf(store->logFile, "%-13s ", "No");
            }

            
            printf("%-10s\n", date);
            fprintf(store->logFile, "%-10s\n", date);

            if (found < maxBooks)
            {
                matchingBooks[found] = current; 
                found++;
            }

        }

        current = current->next;
    }

    if (!found) 
    {
        printf("\nNo matching books found\n");
        fprintf(store->logFile, "\nNo matching books found\n");
        return;
    }

    if (found) 
    {
        
        if (store->currentUserLevel >= PERMISSION_TWO)
        {
            
            printf("\n");
            printf("\n=== update Books ===\n");
            printf("0. Cancel update\n");
            printf("1. Updating books \n\n");
            printf("\nEnter your choice:");
            fprintf(store->logFile, "0. Cancel update\n");
            fprintf(store->logFile, "1. Updating books \n");
            fprintf(store->logFile, "\nEnter your choice(0 - 1) :\n\n");
            fprintf(store->logFile, "\n=== update Books ===\n");

            scanf("%d", &choice);
            while (getchar() != '\n');
            fprintf(store->logFile, "User entered:%d\n", choice);
            
            switch (choice)
            {
            case 0:
                printf("Update cancelled\n");
                fprintf(store->logFile, "Book update cancelled by user\n");
                return;

            case 1:
                for (int i = 0; i < found; i++)
                {
                    handleUpdateBook(store);
                }
                break;

            default:
                printf("Error: Invalid choice! \n");
                fprintf(store->logFile, "Invalid menu choice entered: %d\n", choice);
                return;
            }
        }

        else 
        {
            printf("\nNote: Edit permission required to modify books.\n");
            fprintf(store->logFile, "\nNote: Edit permission required to modify books.\n");
        }
    }
}






//function allows users to search for books based on price. It offers two search options: greater than or less than a specified price.
//It displays the results with the book details and allows updating books if the user has the appropriate permissions. 

void searchByNumber(BookStore* store) 
{
    if (!store)
    {
        printf("No books available to search\n");
        return;
    }

    if (!store->head) 
    {
        printf("No books available to search\n");
        fprintf(store->logFile, "No books available to search\n");
        return;
    }

    int searchChoice;
    double searchPrice;
    int found = 0;
    Book* matchingBooks[100];
    int maxBooks = 100;
    int choice;

    printf("\nSearch by price:\n");
    printf("1. Greater than\n");
    printf("2. Less than\n");
    printf("0. Return to main menu\n\n");

    fprintf(store->logFile, "\nSearch by price:\n");
    fprintf(store->logFile, "1. Greater than\n");
    fprintf(store->logFile, "2. Less than\n");
    fprintf(store->logFile, "0. Return to main menu\n\n");
    
    printf("Enter your choice: ");
    scanf("%d", &searchChoice);

    if (searchChoice == 0)
    {
        return;
    }

    if (searchChoice != 1 && searchChoice != 2 )
    {
        printf("Error: Invalid choice! \n");
        fprintf(store->logFile, "Error: Invalid choice entered: %d\n", searchChoice);
        return;
    }

    fprintf(store->logFile, "Enter your choice (1-2): ");
    fprintf(store->logFile, "%d\n", searchChoice);

    printf("Enter price: ");

    if (scanf("%lf", &searchPrice) != 1 || &searchPrice <= 0)
    {
        printf("Error: Serial number must be a positive number!\n");
        fprintf(store->logFile, "Error: Invalid serial number input\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    fprintf(store->logFile, "Enter price: ");
    fprintf(store->logFile, "User entered:%lf\n", searchPrice);

    

    printf("\nSearch Results:\n");
    printf("%-5s %-60s %-40s %-15s %-10s %-13s %-10s\n", "ID", "Title", "Author", "Genre", "Price", "Bestseller", "Date");
    printf("------------------------------------------------------------------------------------------------------");
    printf("-------------------------------------------------------------\n");


    fprintf(store->logFile, "\nSearch Results:\n");
    fprintf(store->logFile, "%-5s %-60s %-40s %-15s %-10s %-13s %-10s\n", "ID", "Title", "Author", "Genre", "Price", "Bestseller", "Date");
    fprintf(store->logFile, "------------------------------------------------------------------------------------------------------");
    fprintf(store->logFile, "-------------------------------------------------------------\n");

    
    Book* current = store->head;
    while (current)
    {
        int priceMatch = (searchChoice == 1 && current->price > searchPrice) ||
            (searchChoice == 2 && current->price < searchPrice);

        if (priceMatch) 
        {
            char date[11];
            sprintf(date, "%02d/%02d/%04d",
                current->publish_date.day,
                current->publish_date.month,
                current->publish_date.year);

            printf("%-5d %-60s %-40s %-15s %-10.2f ",
                current->serial_number,
                current->title,
                current->author,
                current->genre,
                current->price);


            fprintf(store->logFile, "%-5d %-60s %-40s %-15s %-10.2f ",
                current->serial_number,
                current->title,
                current->author,
                current->genre,
                current->price);

            if (current->is_bestseller == true) 
            {
                printf("%-13s ", "Yes");
                fprintf(store->logFile, "%-13s ", "Yes");
            }

            else 
            {
                printf("%-13s ", "No");
                fprintf(store->logFile, "%-13s ", "No");
            }

            printf("%-10s\n", date);
            fprintf(store->logFile, "%-10s\n", date);

            if (found < maxBooks)
            {
                matchingBooks[found] = current;
                found++;
            }
        }
        current = current->next;
    }

    if (!found) 
    {
        printf("\nNo matching books found\n");
        fprintf(store->logFile, "\nNo matching books found\n");
        return;
    }

    if (found) 
    {
        if (store->currentUserLevel >= PERMISSION_TWO)
        {
            
            printf("\n");
            printf("\n=== update Books ===\n");
            printf("0. Cancel update\n");
            printf("1. Updating books \n\n");
            printf("\nEnter your choice:");
            fprintf(store->logFile, "0. Cancel update\n");
            fprintf(store->logFile, "1. Updating books \n\n");
            fprintf(store->logFile, "\nEnter your choice:\n\n");
            fprintf(store->logFile, "\n=== update Books ===\n");

            scanf("%d", &choice);
            while (getchar() != '\n');
            fprintf(store->logFile, "User entered:%d\n", choice);

            switch (choice)
            {
            case 0:
                printf("Update cancelled\n");
                fprintf(store->logFile, "Book update cancelled by user\n");
                return;

            case 1:
                for (int i = 0; i < found; i++)
                {
                    handleUpdateBook(store);
                }
                break;

            default:
                printf("Error: Invalid choice! \n");
                fprintf(store->logFile, "Invalid menu choice entered: %d\n", choice);
                return;
            }
        }

        else 
        {
            printf("\nNote: Edit permission required to modify books.\n");
            fprintf(store->logFile, "\nNote: Edit permission required to modify books.\n");
        }
    }
}







// The function allows the user to search for books based on their bestseller status. 
// It displays the results with book details and allows updating books if the user has the appropriate permissions.

void searchByBestseller(BookStore* store) 
{


    if (!store )
    {
        printf("No books available to search\n");
        return;
    }

    if (!store->head) 
    {
        printf("No books available to search\n");
        fprintf(store->logFile, "No books available to search\n");
        return;
    }

    int searchChoice;
    int found = 0;
    Book* matchingBooks[100];
    int maxBooks = 100;
    int choice;

    printf("\nSearch by bestseller status:\n");
    printf("1. Show bestsellers only\n");
    printf("2. Show non-bestsellers only\n");
    printf("0. Return to main menu\n\n");

    fprintf(store->logFile, "\nSearch by bestseller status:\n");
    fprintf(store->logFile, "1. Show bestsellers only\n");
    fprintf(store->logFile, "2. Show non-bestsellers only\n");
    fprintf(store->logFile, "0. Return to main menu\n\n");

   
    printf("Enter your choice: ");
    scanf("%d", &searchChoice);

    if (searchChoice == 0)
    {
        return;
    }

    if (searchChoice != 1 && searchChoice != 2 )
    {
        printf("Error: Invalid choice! \n");
        fprintf(store->logFile, "Error: Invalid choice entered: %d\n", searchChoice);
        return;
    }

    fprintf(store->logFile, "Enter your choice (1-2): ");
    fprintf(store->logFile, "User entered:%d\n", searchChoice);

    int searchValue = (searchChoice == 1) ? true : false;
    

    printf("\nSearch Results:\n");
    printf("%-5s %-60s %-40s %-15s %-10s %-13s %-10s\n", "ID", "Title", "Author", "Genre", "Price", "Bestseller", "Date");
    printf("------------------------------------------------------------------------------------------------------");
    printf("-------------------------------------------------------------\n");


    fprintf(store->logFile, "\nSearch Results:\n");
    fprintf(store->logFile, "%-5s %-60s %-40s %-15s %-10s %-13s %-10s\n", "ID", "Title", "Author", "Genre", "Price", "Bestseller", "Date");
    fprintf(store->logFile, "------------------------------------------------------------------------------------------------------");
    fprintf(store->logFile, "-------------------------------------------------------------\n");

    
    Book* current = store->head;
    while (current) 
    {
        if (current->is_bestseller == searchValue)
        {
            char date[11];
            sprintf(date, "%02d/%02d/%04d",
                current->publish_date.day,
                current->publish_date.month,
                current->publish_date.year);

            printf("%-5d %-60s %-40s %-15s %-10.2f ",
                current->serial_number,
                current->title,
                current->author,
                current->genre,
                current->price);


            fprintf(store->logFile, "%-5d %-60s %-40s %-15s %-10.2f ",
                current->serial_number,
                current->title,
                current->author,
                current->genre,
                current->price);


            if (current->is_bestseller == true) 
            {
                printf("%-13s ", "Yes");
                fprintf(store->logFile, "%-13s ", "Yes");
            }
            else 
            {
                printf("%-13s ", "No");
                fprintf(store->logFile, "%-13s ", "No");
            }

            printf("%-10s\n", date);
            fprintf(store->logFile, "%-10s\n", date);


            if (found < maxBooks)
            {
                matchingBooks[found] = current;
                found++;
            }
        }

        current = current->next;
    }

    if (!found) 
    {
        printf("\nNo matching books found\n");
        fprintf(store->logFile, "\nNo matching books found\n");
        return;
    }

    if (found) 
    {
        if (store->currentUserLevel >= PERMISSION_TWO)
        {
            
            printf("\n");
            printf("\n=== update Books ===\n");
            printf("0. Cancel update\n");
            printf("1. Updating books \n\n");
            printf("\nEnter your choice:");
            fprintf(store->logFile, "0. Cancel update\n");
            fprintf(store->logFile, "1. Updating books \n");
            fprintf(store->logFile, "\nEnter your choice(0 - 1) :\n\n");
            fprintf(store->logFile, "\n=== update Books ===\n");

            scanf("%d", &choice);
            while (getchar() != '\n');
            fprintf(store->logFile, "User entered:%d\n", choice);

            switch (choice)
            {
            case 0:
                printf("Update cancelled\n");
                fprintf(store->logFile, "Book update cancelled by user\n");
                return;

            case 1:
                for (int i = 0; i < found; i++)
                {
                    handleUpdateBook(store);
                }
                break;

            default:
                printf("Error: Invalid choice! \n");
                fprintf(store->logFile, "Invalid menu choice entered: %d\n", choice);
                return;
            }
        }

        else 
        {
            printf("\nNote: Edit permission required to modify books.\n");
            fprintf(store->logFile, "Note: Edit permission required to modify books.\n");
        }
    }
}








// The function allows the user to search for books by publication date, either after or before a specific date.
// It displays the results with book details and allows updating books if the user has the appropriate permissions.

void searchByDate(BookStore* store) 
{
    if (!store || !store->head) 
    {
        printf("No books available to search\n");
        fprintf(store->logFile, "No books available to search\n");
        return;
    }

    int searchChoice;
    int day, month, year;
    int found = 0;
    Book* matchingBooks[100];
    int maxBooks = 100;
    int choice;
    
    printf("\nSearch by publication date:\n");
    printf("1. After date\n");
    printf("2. Before date\n");
    printf("0. Return to main menu\n\n");
    fprintf(store->logFile, "\nSearch by publication date:\n");
    fprintf(store->logFile, "1. After date\n");
    fprintf(store->logFile, "2. Before date\n");
    fprintf(store->logFile, "0. Return to main menu\n");

    printf("Enter your choice: ");
    scanf("%d", &searchChoice);

    if (searchChoice == 0 )
    {
        return;
    }

    if (searchChoice != 1 && searchChoice != 2 )
    {
        printf("Error: Invalid choice! \n");
        fprintf(store->logFile, "Error: Invalid choice entered: %d\n", searchChoice);
        return;
    }

    fprintf(store->logFile, "Enter your choice (1-2): ");
    fprintf(store->logFile, "User entered:%d\n", searchChoice);

    
    printf("Enter date (DD MM YYYY): ");
    printf("\n");
    
    printf("Enter day: ");

    if (scanf("%d", &day) != 1 || day <= 0)
    {
        printf("Error: Input must be a number!\n");
        fprintf(store->logFile, "Error: Invalid input - not a number\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    printf("Enter month: ");
    if (scanf("%d", &month) != 1 || month <= 0)
    {
        printf("Error: Input must be a number!\n");
        fprintf(store->logFile, "Error: Invalid input - not a number\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    printf("Enter year: ");
    if (scanf("%d", &year) != 1 || year <= 0)
    {
        printf("Error: Input must be a number!\n");
        fprintf(store->logFile, "Error: Invalid input - not a number\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    fprintf(store->logFile, "Enter date (DD MM YYYY): ");
    fprintf(store->logFile, "\n");
    fprintf(store->logFile, "Enter day:");
    fprintf(store->logFile, "User entered:%d\n", day);
    fprintf(store->logFile, "Enter month:");
    fprintf(store->logFile, "User entered:%d\n", month);
    fprintf(store->logFile, "Enter year:");
    fprintf(store->logFile, "User entered:%d\n", year);



    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900) 
    {
        printf("Invalid date!\n");
        fprintf(store->logFile, "Invalid date!\n");
        return;
    }

    
    printf("\nSearch Results:\n");
    printf("%-5s %-60s %-40s %-15s %-10s %-13s %-10s\n", "ID", "Title", "Author", "Genre", "Price", "Bestseller", "Date");
    printf("------------------------------------------------------------------------------------------------------");
    printf("-------------------------------------------------------------\n");


    fprintf(store->logFile, "\nSearch Results:\n");
    fprintf(store->logFile, "%-5s %-60s %-40s %-15s %-10s %-13s %-10s\n", "ID", "Title", "Author", "Genre", "Price", "Bestseller", "Date");
    fprintf(store->logFile, "------------------------------------------------------------------------------------------------------");
    fprintf(store->logFile, "-------------------------------------------------------------\n");

    
    Book* current = store->head;
    while (current) 
    {
        int isAfter = (current->publish_date.year > year) ||
            (current->publish_date.year == year && current->publish_date.month > month) ||
            (current->publish_date.year == year && current->publish_date.month == month &&
                current->publish_date.day > day);

        int isBefore = (current->publish_date.year < year) ||
            (current->publish_date.year == year && current->publish_date.month < month) ||
            (current->publish_date.year == year && current->publish_date.month == month &&
                current->publish_date.day < day);

        if ((searchChoice == 1 && isAfter) || (searchChoice == 2 && isBefore)) 
        {
            char date[11];
            sprintf(date, "%02d/%02d/%04d",
                current->publish_date.day,
                current->publish_date.month,
                current->publish_date.year);

            printf("%-5d %-60s %-40s %-15s %-10.2f ",
                current->serial_number,
                current->title,
                current->author,
                current->genre,
                current->price);


            fprintf(store->logFile, "%-5d %-60s %-40s %-15s %-10.2f ",
                current->serial_number,
                current->title,
                current->author,
                current->genre,
                current->price);


            if (current->is_bestseller == true) 
            {
                printf("%-13s ", "Yes");
                fprintf(store->logFile, "%-13s ", "Yes");
            }

            else 
            {
                printf("%-13s ", "No");
                fprintf(store->logFile, "%-13s ", "No");
            }

            printf("%-10s\n", date);
            fprintf(store->logFile, "%-10s\n", date);

            if (found < maxBooks)
            {
                matchingBooks[found] = current;
                found++;
            }

        }
        current = current->next;
    }

    if (!found) 
    {
        printf("\nNo matching books found\n");
        fprintf(store->logFile, "\nNo matching books found\n");
        return;
    }

    if (found) 
    {
        if (store->currentUserLevel >= PERMISSION_TWO)
        {
           
            printf("\n");
            printf("\n=== update Books ===\n");
            printf("0. Cancel update\n");
            printf("1. Updating books \n\n");
            printf("\nEnter your choice:");
            fprintf(store->logFile, "0. Cancel update\n");
            fprintf(store->logFile, "1. Updating books \n");
            fprintf(store->logFile, "\nEnter your choice(0 - 1) :\n\n");
            fprintf(store->logFile, "\n=== update Books ===\n");

            scanf("%d", &choice);
            while (getchar() != '\n');
            fprintf(store->logFile, "User entered:%d\n", choice);

            switch (choice)
            {
            case 0:
                printf("Update cancelled\n");
                fprintf(store->logFile, "Book update cancelled by user\n");
                return;

            case 1:
                for (int i = 0; i < found; i++)
                {
                    handleUpdateBook(store);
                }
                break;

            default:
                printf("Error: Invalid choice! \n");
                fprintf(store->logFile, "Invalid menu choice entered: %d\n", choice);
                return;
            }
        }

        else 
        {
            printf("\nNote: Edit permission required to modify books.\n");
            fprintf(store->logFile, "\nNote: Edit permission required to modify books.\n");
        }
    }
}










// The function presents a menu to the user for searching books based on different criteria: text (title/author),
// price, bestseller status, or publication date. It logs the user's choices and actions.

void handleSearchBooks(BookStore* store)
{

    if (!store)
    {
        printf("No books available to search\n");
        return;
    }


    if (!store->head)
    {
        printf("No books available to search\n");
        fprintf(store->logFile, "No books available to search\n");
        return;
    }

    int choice;

    printf("\n=== Search Books ===\n");
    fprintf(store->logFile, "\n=== Search Books ===\n");

    printf("1. Search by text (title/author)\n");
    fprintf(store->logFile, "1. Search by text (title/author)\n");

    printf("2. Search by price\n");
    fprintf(store->logFile, "2. Search by price\n");

    printf("3. Search by bestseller status\n");
    fprintf(store->logFile, "3. Search by bestseller status\n");

    printf("4. Search by publication date\n");
    fprintf(store->logFile, "4. Search by publication date\n");

    printf("0. Return to main menu\n");
    fprintf(store->logFile, "0. Return to main menu\n");

    printf("\nEnter your choice: ");
    scanf("%d", &choice);
    fprintf(store->logFile, "Enter your choice: ");
    fprintf(store->logFile, "User entered:%d\n", choice);

    while (getchar() != '\n');

    switch (choice)
    {
    case 0:
        return;

    case 1:
        searchByText(store);
        break;

    case 2:
        searchByNumber(store);
        break;

    case 3:
        searchByBestseller(store);
        break;

    case 4:
        searchByDate(store);
        break;

    default:
        printf("Error: Invalid choice! \n");
        fprintf(store->logFile, "Invalid menu choice entered: %d\n", choice);
        return;
    }
}














// The function handles the user input for selecting an option from the menu.
// It validates that the input is a positive number and checks the user's permission level for specific options.
// If the input is invalid or the user doesn't have the necessary permissions, the function prints an error message and returns 0.

int handleUserInput(int* choice, BookStore* store) 
{
    if (!store) return 0;

    printf("\nEnter your choice: ");
    fprintf(store->logFile, "Enter your choice:");

    
    if (scanf("%d", choice) != 1)
    {
        printf("Invalid input. Please enter a number.\n");
        fprintf(store->logFile, "Invalid input. Please enter a number.\n");
        while (getchar() != '\n'); 
        return 0;
    }

    fprintf(store->logFile, " User entered:%d\n", *choice);

    if (*choice < 0) 
    {
        printf("Invalid choice. Please enter a positive number.\n");
        fprintf(store->logFile, "Invalid choice. Please enter a positive number.\n");
        return 0;
    }

    return 1; 
}







// The function displays the main menu of the Book Store Management System.
// It shows different options based on the user's access level and logs the displayed menu to a file.

void displayMenu(BookStore* store)
{
    if (!store) return;

    printf("\n=== Book Store Management System ===\n");
    printf("Current user: %s (Level: %d)\n\n", store->currentUsername, store->currentUserLevel);

    fprintf(store->logFile, "\n=== Book Store Management System ===\n");
    fprintf(store->logFile, "\n=== Displaying Menu ===\n");
    fprintf(store->logFile, "Menu displayed for user: %s (Level: %d)\n", store->currentUsername, store->currentUserLevel);

    printf("1. View all books\n");
    fprintf(store->logFile, "1. View all books\n");

    printf("2. Search books\n");
    fprintf(store->logFile, "2. Search books\n");

    printf("3. Add new book\n");
    fprintf(store->logFile, "3. Add new book\n");

    printf("4. Customer Management\n");
    fprintf(store->logFile, "9. Customer Management\n");

    if (store->currentUserLevel >= PERMISSION_TWO)
    {
        printf("5. Update book\n");
        fprintf(store->logFile, "4. Update book\n");

        printf("6. Delete book\n");
        fprintf(store->logFile, "5. Delete book\n");
    }


    if (store->currentUserLevel == PERMISSION_MANAGER)
    {
        printf("7. View all users\n");
        fprintf(store->logFile, "6. View all users\n");

        printf("8. Add new user\n");
        fprintf(store->logFile, "7. Add new user\n");

        printf("9. Delete user\n");
        fprintf(store->logFile, "8. Delete user\n");
    }


    printf("\n0. Exit\n");
    fprintf(store->logFile, "\n0. Exit\n");
    fprintf(store->logFile, "Menu display completed\n");
}













void saveToFile(BookStore* store)
{
    FILE* fp = fopen(store->itemsPath, "w");
    if (fp)
    {
        fprintf(fp, "%-5s %-60s %-40s %-15s %-10s %-13s %-10s\n", "ID", "Title", "Author", "Genre", "Price", "Bestseller", "Date");
        fprintf(fp, "------------------------------------------------------------------------------------------------------");
        fprintf(fp, "-------------------------------------------------------------\n");


        Book* current = store->head;
        while (current != NULL)
        {
            char date[11];
            snprintf(date, sizeof(date), "%02d/%02d/%04d",
                current->publish_date.day,
                current->publish_date.month,
                current->publish_date.year);

            fprintf(fp, "%-5d %-60s %-40s %-15s %-10.2f %-13s %-10s\n",
                current->serial_number,
                current->title,
                current->author,
                current->genre,
                current->price,
                current->is_bestseller ? "Yes" : "No",
                date);

            current = current->next;
        }
        fclose(fp);
    }

}














void initCustomerSystem(BookStore* store) 
{
    if (!store) 
    {
        printf("Error: Invalid store reference\n");
        return;
    }

    if (!store->customerPath) 
    {
        printf("Error: Customer file path not initialized\n");
        fprintf(store->logFile, "Error: Customer file path not initialized in initCustomerSystem\n");
        return;
    }

    fprintf(store->logFile, "\n=== Initializing Customer System ===\n");

    
    FILE* fp = fopen(store->customerPath, "r");
    if (!fp) 
    {
        
        fp = fopen(store->customerPath, "w");
        if (!fp) 
        {
            printf("Error: Cannot create customers file\n");
            fprintf(store->logFile, "Error: Cannot create customers file: %s\n", store->customerPath);
            return;
        }

       
        fprintf(fp, "%-5s %-30s %-15s %-50s\n", "ID", "Name", "Phone", "Email");
        fprintf(fp, "--------------------------------------------------------------------------------\n");
        fclose(fp);

        fprintf(store->logFile, "Created new customers file with headers\n");
    }
    else 
    {
        fclose(fp);
        fprintf(store->logFile, "Existing customers file found\n");
    }

    
    store->customerHead = NULL;

    fprintf(store->logFile, "Customer system initialization completed\n");
}






void loadCustomersFromFile(BookStore* store) 
{
    if (!store) 
    {
        printf("Error: Invalid store reference\n");
        fprintf(store->logFile, "Error: Invalid store reference\n");
        return;
    }

    FILE* fp = fopen(store->customerPath, "r");
    if (!fp) 
    {
        printf("No customers file found - starting with empty list\n");
        fprintf(store->logFile, "No customers file found - starting with empty list\n");
        return;
    }

    fprintf(store->logFile, "\n=== Loading Customers from File ===\n");

    char line[500];
    fgets(line, sizeof(line), fp);  
    fgets(line, sizeof(line), fp);  

    
    Customer* current = store->customerHead;
    while (current != NULL) 
    {
        Customer* temp = current;
        current = current->next;
        free(temp);
    }
    store->customerHead = NULL;

    char name[MAX_FULLNAME] = { 0 };
    char phone[MAX_PHONE] = { 0 };
    char email[MAX_EMAIL] = { 0 };
    int id;
    int customersLoaded = 0;

    while (fgets(line, sizeof(line), fp)) 
    {
        if (sscanf(line, "%d %29[^\t] %14[^ ] %49[^\n]",
            &id, name, phone, email) == 4) 
        {

            Customer* newCustomer = (Customer*)malloc(sizeof(Customer));
            if (!newCustomer) {
                printf("Error: Memory allocation failed\n");
                fprintf(store->logFile, "Error: Memory allocation failed\n");
                fclose(fp);
                return;
            }

            newCustomer->id = id;
            strcpy(newCustomer->name, name);
            strcpy(newCustomer->phone, phone);
            strcpy(newCustomer->email, email);
            newCustomer->next = NULL;

            if (store->customerHead == NULL) 
            {
                store->customerHead = newCustomer;
            }
            else 
            {
                newCustomer->next = store->customerHead;
                store->customerHead = newCustomer;
            }

            customersLoaded++;
        }
    }

    fclose(fp);
    fprintf(store->logFile, "Total customers loaded: %d\n", customersLoaded);
}





void displayCustomers(BookStore* store) 
{
    if (!store) 
    {
        printf("Error: Invalid store reference\n");
        return;
    }

    Customer* current = store->customerHead;
    int customerCount = 0;

    printf("\n=== Customers List ===\n");
    printf("\n");
    fprintf(store->logFile, "\n=== Customers List ===\n");
    fprintf(store->logFile, "\n");

    printf("%-5s %-30s %-15s %-50s\n", "ID", "Name", "Phone", "Email");
    printf("--------------------------------------------------------------------------------\n");
    fprintf(store->logFile, "%-5s %-30s %-15s %-50s\n", "ID", "Name", "Phone", "Email");
    fprintf(store->logFile, "--------------------------------------------------------------------------------\n");

    while (current != NULL) 
    {
        printf("%-5d %-30s %-15s %-50s\n",
            current->id,
            current->name,
            current->phone,
            current->email);

        fprintf(store->logFile, "%-5d %-30s %-15s %-50s\n",
            current->id,
            current->name,
            current->phone,
            current->email);

        customerCount++;
        current = current->next;
    }

    printf("\nTotal customers: %d\n", customerCount);
    fprintf(store->logFile, "\nTotal customers: %d\n", customerCount);
}





void handleAddCustomer(BookStore* store) 
{
    if (!store) 
    {
        printf("Error: Invalid store reference\n");
        fprintf(store->logFile, "Error: Invalid store reference\n");
        return;
    }

    Customer* newCustomer = (Customer*)malloc(sizeof(Customer));
    if (!newCustomer)
    {
        printf("Error: Memory allocation failed\n");
        fprintf(store->logFile, "Error: Memory allocation failed\n");
        return;
    }

    printf("\n=== Add New Customer ===\n");
    fprintf(store->logFile, "\n=== Add New Customer ===\n");

    int maxId = 0;
    Customer* current = store->customerHead;
    while (current) 
    {
        if (current->id > maxId) maxId = current->id;
        current = current->next;
    }
    newCustomer->id = maxId + 1;

    printf("Enter customer name: ");
    fgets(newCustomer->name, MAX_FULLNAME, stdin);
    newCustomer->name[strcspn(newCustomer->name, "\n")] = 0;
    fprintf(store->logFile, "Enter customer name: ");
    fprintf(store->logFile, "User entered: %s\n", newCustomer->name);

    printf("Enter phone number: ");
    fgets(newCustomer->phone, MAX_PHONE, stdin);
    newCustomer->phone[strcspn(newCustomer->phone, "\n")] = 0;
    fprintf(store->logFile, "Enter phone number: ");
    fprintf(store->logFile, "User entered: %s\n", newCustomer->phone);

    printf("Enter email: ");
    fgets(newCustomer->email, MAX_EMAIL, stdin);
    newCustomer->email[strcspn(newCustomer->email, "\n")] = 0;
    fprintf(store->logFile, "Enter email: ");
    fprintf(store->logFile, "User entered: %s\n", newCustomer->email);

    newCustomer->next = store->customerHead;
    store->customerHead = newCustomer;

    printf("\nCustomer added successfully! ID: %d\n", newCustomer->id);
    fprintf(store->logFile, "\nCustomer added successfully! ID: %d\n", newCustomer->id);

    saveCustomersToFile(store);
}







Customer* findCustomerById(BookStore* store, int id) 
{
    if (!store) 
    {
        printf("Error: Invalid store reference\n");
        return NULL;
    }

    printf("\n=== Find Customer ===\n");
    fprintf(store->logFile, "\n=== Find Customer ===\n");

    Customer* current = store->customerHead;
    while (current) 
    {
        if (current->id == id) 
        {
            printf("Customer found:\n");
            printf("ID: %d\n", current->id);
            printf("Name: %s\n", current->name);
            printf("Phone: %s\n", current->phone);
            printf("Email: %s\n", current->email);

            fprintf(store->logFile, "Customer found:\n");
            fprintf(store->logFile, "ID: %d\n", current->id);
            fprintf(store->logFile, "Name: %s\n", current->name);
            fprintf(store->logFile, "Phone: %s\n", current->phone);
            fprintf(store->logFile, "Email: %s\n", current->email);

            return current;
        }
        current = current->next;
    }

    printf("Customer not found with ID: %d\n", id);
   /* fprintf(store->logFile, "Customer not found with ID:%d\n", id);*/
    return NULL;
}






void findCustomersByName(BookStore* store) 
{
    if (!store) 
    {
        printf("Error: Invalid store reference\n");
        return;
    }

    char searchName[MAX_FULLNAME];
    int found = 0;

    printf("\n=== Search Customers by Name ===\n");
    fprintf(store->logFile, "\n=== Search Customers by Name ===\n");

    printf("Enter name to search: ");
    fgets(searchName, MAX_FULLNAME, stdin);
    searchName[strcspn(searchName, "\n")] = 0;  

    fprintf(store->logFile, "Searching for name: %s\n", searchName);

    printf("\nSearch Results:\n");
    printf("%-5s %-30s %-15s %-50s\n", "ID", "Name", "Phone", "Email");
    printf("--------------------------------------------------------------------------------\n");

    fprintf(store->logFile, "\nSearch Results:\n");
    fprintf(store->logFile, "%-5s %-30s %-15s %-50s\n", "ID", "Name", "Phone", "Email");
    fprintf(store->logFile, "--------------------------------------------------------------------------------\n");

    Customer* current = store->customerHead;
    while (current != NULL) 
    {
        
        if (strstr(current->name, searchName) != NULL) 
        {
            printf("%-5d %-30s %-15s %-50s\n",
                current->id,
                current->name,
                current->phone,
                current->email);

            fprintf(store->logFile, "%-5d %-30s %-15s %-50s\n",
                current->id,
                current->name,
                current->phone,
                current->email);

            found++;
        }
        current = current->next;
    }

    if (found == 0) 
    {
        printf("\nNo customers found with name containing: %s\n", searchName);
        fprintf(store->logFile, "No customers found with name containing: %s\n", searchName);
    }
    else 
    {
        printf("\nTotal matches found: %d\n", found);
        fprintf(store->logFile, "Total matches found: %d\n", found);
    }
}



void handleDeleteCustomer(BookStore* store) 
{
    if (!store) 
    {
        printf("Error: Invalid store reference\n");
        return;
    }

    int id;
    char confirm;

    printf("\n=== Delete Customer ===\n");
    fprintf(store->logFile, "\n=== Delete Customer ===\n");

    printf("Enter customer ID to delete: ");
    if (scanf("%d", &id) != 1) 
    {
        printf("Error: Invalid ID input\n");
        fprintf(store->logFile, "Error: Invalid ID input\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    fprintf(store->logFile, "Enter customer ID to delete: ");
    fprintf(store->logFile, "User entered: %d\n", id);

    Customer* customerToDelete = findCustomerById(store, id);
    if (!customerToDelete)
    {
        return;
    }

    printf("\nAre you sure you want to delete this customer? (y/n): ");
    scanf(" %c", &confirm);
    while (getchar() != '\n');

    fprintf(store->logFile, "Delete confirmation requested (y/n): ");
    fprintf(store->logFile, "User entered: %c\n", confirm);

    if (confirm == 'y' || confirm == 'Y')
    {
        if (customerToDelete == store->customerHead)
        {
            store->customerHead = customerToDelete->next;
        }
        else 
        {
            Customer* prev = store->customerHead;
            while (prev->next != customerToDelete) 
            {
                prev = prev->next;
            }
            prev->next = customerToDelete->next;
        }

        free(customerToDelete);
        printf("Customer deleted successfully!\n");
        fprintf(store->logFile, "Customer deleted successfully!\n");

        saveCustomersToFile(store);
    }
    else
    {
        printf("Delete operation cancelled\n");
        fprintf(store->logFile, "Delete operation cancelled\n");
    }
}





void saveCustomersToFile(BookStore* store) 
{
    if (!store) 
    {
        printf("Error: Invalid store reference\n");
        return;
    }

    FILE* fp = fopen(store->customerPath, "w");
    if (!fp) 
    {
        printf("Error opening customers file for writing!\n");
        fprintf(store->logFile, "Error opening customers file for writing!\n");
        return;
    }

    fprintf(store->logFile, "\n=== Saving Customers to File ===\n");

    fprintf(fp, "%-5s %-30s %-15s %-50s\n", "ID", "Name", "Phone", "Email");
    fprintf(fp, "--------------------------------------------------------------------------------\n");

    Customer* current = store->customerHead;
    int customerCount = 0;

    while (current != NULL) 
    {
        fprintf(fp, "%-5d %-30s %-15s %-50s\n",
            current->id,
            current->name,
            current->phone,
            current->email);

        customerCount++;
        current = current->next;
    }

    fclose(fp);
    printf("Customers file saved successfully!\n");
    fprintf(store->logFile, "Customers file saved successfully. Total customers saved: %d\n", customerCount);
}





void handleCustomerManagement(BookStore* store) 
{
    if (!store) return;
     
    int customerChoice;
    do {
        
        printf("\n=== Customer Management ===\n");
        printf("1. Display all customers\n");
        printf("2. Add new customer\n");
        printf("3. Find customer by ID\n");
        printf("4. Find customer by Name\n");  
        printf("5. Delete customer\n");
        printf("0. Back to main menu\n");

        fprintf(store->logFile, "\n=== Customer Management Menu ===\n");
        fprintf(store->logFile, "1. Display all customers\n");
        fprintf(store->logFile, "2. Add new customer\n");
        fprintf(store->logFile, "3. Find customer by ID\n");
        fprintf(store->logFile, "4. Find customer by Name\n");
        fprintf(store->logFile, "5. Delete customer\n");
        fprintf(store->logFile, "0. Back to main menu\n");
        
        fprintf(store->logFile, "Menu displayed for user: %s\n", store->currentUsername);

        printf("\nEnter your choice: ");
        if (scanf("%d", &customerChoice) != 1) 
        {
            printf("Error: Invalid input\n");
            fprintf(store->logFile, "Error: Invalid input in customer menu\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        fprintf(store->logFile, "User selected option: %d\n", customerChoice);

        switch (customerChoice) 
        {
        case 0:
            printf("Returning to main menu\n");
            fprintf(store->logFile, "User exited customer management menu\n");
            break;

        case 1:
            displayCustomers(store);
            break;

        case 2:
            handleAddCustomer(store);
            displayCustomers(store);
            break;

        case 3:
            printf("Enter customer ID to find: ");
            fprintf(store->logFile, "Customer search requested\n");

            int searchId;
            if (scanf("%d", &searchId) != 1) 
            {
                printf("Error: Invalid ID input\n");
                fprintf(store->logFile, "Error: Invalid ID input for customer search\n");
                while (getchar() != '\n');
                break;
            }
            while (getchar() != '\n');

            fprintf(store->logFile, "Searching for customer ID: %d\n", searchId);
            findCustomerById(store, searchId);
            break;

        case 4:
            findCustomersByName(store);  
            break;

        case 5:
            handleDeleteCustomer(store);
            displayCustomers(store);
            break;

        default:
            printf("Error: Invalid choice!\n");
            fprintf(store->logFile, "Error: Invalid choice in customer menu: %d\n",
                customerChoice);
        }
    } while (customerChoice != 0);
}











// This function performs the cleanup of system resources when the application shuts down. It starts by freeing all dynamically allocated book records
// from the linked list and then logs the total number of books freed.

void cleanup(BookStore* store)
{

    if (!store) return;

    int day, month, year, hours, mins;
    Book* current = store->head;
    int booksFreed = 0;

    printf("\nCleaning up system resources...\n");
    fprintf(store->logFile, "\nCleaning up system resources...\n");



    while (current != NULL)
    {
        Book* next = current->next;
        free(current);
        booksFreed++;
        current = next;
    }

    printf("Total books freed: %d\n", booksFreed);
    fprintf(store->logFile, "Total books freed: %d\n", booksFreed);

    Customer* currentCustomer = store->customerHead;
    while (currentCustomer != NULL)
    {
        Customer* next = currentCustomer->next;
        free(currentCustomer);
        currentCustomer = next;
    }

    store->head = NULL;
    store->tail = NULL;
    store->current = NULL;

    if (store->logFile)
    {
        getDateTime(&day, &month, &year, &hours, &mins);
        fprintf(store->logFile, "=== System shutdown completed at: %02d/%02d/%d, %02d:%02d ===\n\n", day, month, year, hours, mins);
        fclose(store->logFile);
        store->logFile = NULL;
    }

    if (store->customerPath)
    {
        free(store->customerPath);
        store->customerPath = NULL;
    }

    if (store->usersPath)
    {
        free(store->usersPath);
        store->usersPath = NULL;
    }
    if (store->itemsPath)
    {
        free(store->itemsPath);
        store->itemsPath = NULL;
    }
    if (store->logPath)
    {
        free(store->logPath);
        store->logPath = NULL;
    }
}
