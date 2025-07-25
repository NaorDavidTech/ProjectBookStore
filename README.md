

# 📚 Bookstore Management System in C

This project implements a full-featured **bookstore management system** using **C programming language**, supporting users with multiple permission levels, dynamic book handling, and persistent data storage.

---

## 🚀 Features

- 📖 **Book Management**
  - View all books
  - Search books by title, author, or genre
  - Add, update, or delete books (Admin only)
  - Bestseller flag and publication date

- 👤 **User System**
  - Login with 3 permission levels:
    - Level 3: System Manager (Full access)
    - Level 2: Employee (Add/search books)
    - Level 1: Customer (Search only)
  - View and manage users (Admins only)

- 🛒 **Customer Management**
  - Load and display customer data from `customer.txt`
  - Search customer info


- 📁 **Persistent Data Storage**
  - Books saved in `items.txt`
  - Users stored in `users.txt`
  - Customers stored in `customer.txt`
  - Logs saved in `log.txt`

---

## 📁 Files Included

- `main.c` – Main menu logic and system control
- `bookstore.c` – Core functionality for book/user/customer management
- `bookstore.h` – Function declarations and data structures
- `items.txt` – Sample book database (20+ titles)
- `users.txt` – System users (admin + test users)
- `customer.txt` – Customer records (ID, name, phone, email)
- `log.txt` – Sample session logs with time-stamped actions

---

## 🧪 Sample Users

| Username | Password | Level | Name         |
|----------|----------|-------|--------------|
| admin    | admin    | 3     | Manager_System |


---

## 🧠 How It Works

1. On startup, the system loads books, users, and customers from text files.
2. User logs in with credentials (up to 3 attempts).
3. Menu is displayed based on user level.
4. Every action is logged in `log.txt`.
5. On exit, memory is cleared and logs finalized.

---


## 👨‍🔧 Author

Naor David   
