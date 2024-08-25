
#### README.md

```markdown
# Running PostgreSQL Locally with test_all_dependencies

This guide provides instructions for setting up and running a local PostgreSQL server to test the `test_all_dependencies` program, which performs various tests including interactions with PostgreSQL, libsodium, libpq, and MySQL.

## Prerequisites

- **Homebrew**: Ensure that Homebrew is installed on your system.
- **PostgreSQL**: Install PostgreSQL version 14 using Homebrew.
- **Libsodium, libpq, and MySQL**: Install necessary libraries if not already installed.

## Installation Steps

### 1. Install PostgreSQL

First, install PostgreSQL version 14:

```bash
brew install postgresql@14
```

### 2. Initialize the Database Cluster

Once PostgreSQL is installed, initialize the database cluster:

```bash
/opt/homebrew/opt/postgresql@14/bin/initdb /opt/homebrew/var/postgresql@14
```

### 3. Start PostgreSQL

Start the PostgreSQL service:

```bash
brew services start postgresql@14
```

### 4. Verify the PostgreSQL Installation

To ensure PostgreSQL is running correctly, list the services:

```bash
brew services list
```

You should see `postgresql@14` listed as `started`.

### 5. Create a Database and Role

1. **Connect to the PostgreSQL server:**

   ```bash
   psql -U postgres -h /tmp -d postgres
   ```

2. **Create the `cserver` database:**

   ```sql
   CREATE DATABASE cserver;
   ```

3. **(Optional) Create the `cserver` role:**

   ```sql
   CREATE ROLE cserver WITH LOGIN PASSWORD 'your_password';
   ```

4. **Exit the `psql` shell:**

   ```sql
   \q
   ```

### 6. Modify and Compile the Code

Update the `libpq` connection string in the code if necessary. Compile the program using `clang`:

```bash
clang -g test.c -o test_all_dependencies \
    -I/opt/homebrew/opt/libmicrohttpd/include \
    -I/opt/homebrew/opt/libsodium/include \
    -I/opt/homebrew/opt/libpq/include \
    -I/opt/homebrew/opt/mysql-client/include/mysql \
    -L/opt/homebrew/opt/libmicrohttpd/lib \
    -L/opt/homebrew/opt/libsodium/lib \
    -L/opt/homebrew/opt/libpq/lib \
    -L/opt/homebrew/opt/mysql-client/lib \
    -lmicrohttpd -lsodium -lpq -lmysqlclient
```

### 7. Run the Program

Run the compiled `test_all_dependencies` program:

```bash
./test_all_dependencies
```

You should see output indicating that each test succeeded:

```plaintext
Daemon started successfully on port 8080
libsodium: Success
libsodium: Password hashing success
libpq: Success
mysql-client: Success
All tests succeeded.
```

### Troubleshooting

- If you encounter errors with the PostgreSQL connection, ensure the database and role exist and match your connection string.
- If the `postgres` role doesn't exist, create it using the instructions in step 5.

### License

This project is licensed under the [MIT License](LICENSE).

```

This README provides a step-by-step guide for setting up and running PostgreSQL locally, creating necessary roles and databases, and compiling and running the `test_all_dependencies` program. The annotated code explains the functionality of each part of the program.