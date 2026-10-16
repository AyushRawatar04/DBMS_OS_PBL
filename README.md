# Cinema Ticket Booking System

A multi-threaded C++ Cinema Ticket Booking System integrating MySQL database storage, process management, and POSIX signal handling.

---

## Table of Contents
1. [Features & OS Concepts](#features--os-concepts)
2. [Prerequisites](#prerequisites)
3. [Installing MySQL & MySQL Workbench](#installing-mysql--mysql-workbench)
4. [Setting Up Connection in MySQL Workbench](#setting-up-connection-in-mysql-workbench)
5. [Creating Database & Tables](#creating-database--tables)
6. [Database Credentials in Code](#database-credentials-in-code)
7. [Compiling & Running](#compiling--running)
8. [Troubleshooting & Common Errors](#troubleshooting--common-errors)

---

## Features & OS Concepts

- **Multithreading (`std::thread`, `std::future`)**: Parallel loading of movies and show details from MySQL on startup.
- **Process Isolation (`fork`, `waitpid`)**: Ticket printing runs in an isolated child process to avoid blocking the main booking thread.
- **Graceful Shutdown (`SIGINT`)**: Intercepts `Ctrl + C` signals to safely flush logs and close active database connections.
- **Database Singleton**: Thread-safe database resource management using MySQL C API (`libmysqlclient`).
- **Activity Logging**: Centralized logging mechanism writing timestamped system events to `booking.log`.

---

## Prerequisites

- **macOS** or **Linux**
- **C++17** compatible compiler (`g++` or `clang++`)
- **Make** build utility
- **MySQL Server** and **MySQL C Client library** (`mysql`)
- **MySQL Workbench** (optional GUI client)

---

## Installing MySQL & MySQL Workbench

### On macOS (Homebrew)

1. **Install MySQL Server & C Library**:
   ```bash
   brew install mysql
   ```

2. **Start the MySQL Service**:
   ```bash
   brew services start mysql
   ```

3. **Install MySQL Workbench**:
   ```bash
   brew install --cask mysql-workbench
   ```
   *(Or download the `.dmg` directly from the [MySQL Official Website](https://dev.mysql.com/downloads/workbench/)).*

---

## Setting Up Connection in MySQL Workbench

1. Launch **MySQL Workbench**.
2. Click the **`+`** icon next to **MySQL Connections** on the welcome screen.
3. Configure the connection parameters:
   - **Connection Name**: `Local Cinema DB` (or any label)
   - **Hostname**: `127.0.0.1` (or `localhost`)
   - **Port**: `3306`
   - **Username**: `root`
   - **Password**: Click *Store in Keychain/Vault* (leave blank if your local root user has no password, or enter your MySQL root password).
4. Click **Test Connection**. You should see:
   > *Successfully made the MySQL connection.*
5. Click **OK** to save and double-click the connection tile to open the query editor.

---

## Creating Database & Tables

The project includes a ready-to-run database setup script: [`schema.sql`](file:///Users/ayushrawat/Documents/TicketBookingSystem_segregated%20copy/schema.sql).

### Method 1: Using MySQL Workbench (GUI)
1. In MySQL Workbench, go to **File** → **Open SQL Script...** (or press `Cmd + O`).
2. Select the [`schema.sql`](file:///Users/ayushrawat/Documents/TicketBookingSystem_segregated%20copy/schema.sql) file from the project directory.
3. Click the **Execute (Lightning Bolt ⚡)** icon in the toolbar to run the entire script.
4. In the left **Schemas** panel, right-click and choose **Refresh All**. You will see `cinema_booking` with the following tables:
   - `movies`
   - `screens`
   - `seats`
   - `shows`
   - `show_seats`
   - `customers`
   - `bookings`
   - `booking_seats`
   - `payments`

### Method 2: Using Terminal (CLI)
You can directly pipe the schema into MySQL:

```bash
# If your root user has no password:
mysql -u root < schema.sql

# If your root user requires a password:
mysql -u root -p < schema.sql
```

To verify:
```bash
mysql -u root -e "USE cinema_booking; SHOW TABLES;"
```

---

## Database Credentials in Code

The system connects to MySQL using the credentials defined in:
- [`DBConnection.cpp`](file:///Users/ayushrawat/Documents/TicketBookingSystem_segregated%20copy/DBConnection.cpp#L14)
- [`Main.cpp`](file:///Users/ayushrawat/Documents/TicketBookingSystem_segregated%20copy/Main.cpp#L42) (for parallel loading threads)

By default, the connection string is:
```cpp
mysql_real_connect(conn, "localhost", "root", "", "cinema_booking", 3306, nullptr, 0);
```

> **Note**: If your MySQL `root` user has a password, change `""` to `"your_password"` in both [`DBConnection.cpp`](file:///Users/ayushrawat/Documents/TicketBookingSystem_segregated%20copy/DBConnection.cpp) and [`Main.cpp`](file:///Users/ayushrawat/Documents/TicketBookingSystem_segregated%20copy/Main.cpp).

---

## Compiling & Running

### Recommended: Using `make`

The project includes a [`Makefile`](file:///Users/ayushrawat/Documents/TicketBookingSystem_segregated%20copy/Makefile) that handles all source files, include paths, and library links.

1. **Clean and Compile**:
   ```bash
   make clean && make
   ```

2. **Run the Executable**:
   ```bash
   ./TicketBookingSystem
   ```

---

### Alternative: Compile Manually with `g++`

If compiling without `make`, pass all `.cpp` files together with the MySQL include and library directories:

```bash
g++ -std=c++17 \
  -I/opt/homebrew/Cellar/mysql/26.7.0_1/include \
  *.cpp \
  -L/opt/homebrew/Cellar/mysql/26.7.0_1/lib \
  -L/opt/homebrew/lib \
  -lmysqlclient -lz -lssl -lcrypto -lresolv \
  -o TicketBookingSystem
```

Run:
```bash
./TicketBookingSystem
```

---

## Troubleshooting & Common Errors

### 1. `Use of undeclared identifier 'mysql_init'` or `'mysql/mysql.h' file not found`
- **Cause**: The compiler or IDE language server (clangd / IntelliSense) does not know where Homebrew installed MySQL headers.
- **Solution**:
  - In code, use `#include <mysql/mysql.h>`.
  - Ensure compiler flag `-I/opt/homebrew/Cellar/mysql/<version>/include` is passed.
  - The workspace includes [`compile_commands.json`](file:///Users/ayushrawat/Documents/TicketBookingSystem_segregated%20copy/compile_commands.json) and [`.clangd`](file:///Users/ayushrawat/Documents/TicketBookingSystem_segregated%20copy/.clangd) to automatically fix IDE squiggly lines.

### 2. `zsh: permission denied: ./Main.o`
- **Cause**: `Main.o` is an intermediate object file compiled with `-c`, not a runnable binary.
- **Solution**: Run the compiled target executable `./TicketBookingSystem`, not `./Main.o`.

### 3. `[DB] Connection failed: Access denied for user 'root'@'localhost'`
- **Cause**: MySQL root password mismatch.
- **Solution**: Update the password parameter in [`DBConnection.cpp:L14`](file:///Users/ayushrawat/Documents/TicketBookingSystem_segregated%20copy/DBConnection.cpp#L14) and [`Main.cpp:L42,L83`](file:///Users/ayushrawat/Documents/TicketBookingSystem_segregated%20copy/Main.cpp#L42) to match your local MySQL root password.

### 4. `Can't connect to local MySQL server through socket`
- **Cause**: MySQL server service is not running.
- **Solution**: Start the service with:
  ```bash
  brew services start mysql
  ```
