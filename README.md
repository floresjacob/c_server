
#### README.md
# C Web Server with JWT Authentication

This project is a simple web server implemented in C, using the `libmicrohttpd` library for handling HTTP requests and `libsodium` for password hashing. It supports user registration, login, and a protected route accessible only with a valid JSON Web Token (JWT).

## Features

- **User Registration**: Users can register with a username, email, and password.
- **User Login**: Users can log in with their credentials and receive a JWT.
- **Protected Route**: A route accessible only to authenticated users who provide a valid JWT.

## Project Structure

- **src/**: Contains all the source files for the project.
  - `main.c`: Entry point for the server.
  - `routes.c`: Defines the routes for the server (e.g., `/register`, `/login`, `/protected`).
  - `utils.c`: Utility functions for password hashing, sending HTTP responses, etc.
  - `jwt.c`: Functions for generating and verifying JWTs.
  - `db.c`: Database-related functions, including user registration and login.

- **include/**: Contains header files for the project.
  - `project_jwt.h`: Header file for JWT-related declarations.
  - `db.h`: Header file for database-related functions.
  - `utils.h`: Header file for utility functions.

## Prerequisites

- **libmicrohttpd**: For handling HTTP requests.
- **libsodium**: For cryptographic functions, including password hashing.
- **libpq**: For interacting with a PostgreSQL database.
- **libjwt**: For generating and verifying JWTs.
- **PostgreSQL**: Database server to store user information.

You can install the required libraries using Homebrew:

```bash
brew install libmicrohttpd libsodium libpq libjwt postgresql
```

## Setup

### 1. Clone the Repository

```bash
git clone https://github.com/your-username/c_server.git
cd c_server
```

### 2. Initialize the Database

Ensure PostgreSQL is running and accessible. Create the necessary database and table:

```sql
CREATE DATABASE c_server;
\c c_server;

CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

Update the `connect_db` function in `db.c` if your database configuration differs.

### 3. Build the Project

Compile the server using `make`:

```bash
make
```

### 4. Run the Server

Start the server:

```bash
./build/my_c_server
```

The server will start on port `8080`.

## Usage

### Register a User

```bash
curl -X POST http://localhost:8080/register \
-d "username=testuser&email=testuser@example.com&password=testpassword"
```

### Log In

```bash
curl -X POST http://localhost:8080/login \
-H "Content-Type: application/x-www-form-urlencoded" \
-d "username=testuser&password=testpassword"
```

This will return a JWT token.

### Access Protected Route

Use the JWT token received during login to access the protected route:

```bash
curl -X GET http://localhost:8080/protected \
-H "Authorization: Bearer YOUR_JWT_TOKEN"
```

## Debugging

If you encounter issues with the server, check the console logs for output. Common issues include:

- **Database connection problems**: Ensure the PostgreSQL server is running and accessible.
- **JWT verification failures**: Check that the JWT is being sent correctly in the Authorization header.

## Contributing

Feel free to open issues or submit pull requests if you find bugs or have suggestions for improvements.


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
    -lmicrohttpd -lsodium -lpq -lmysqlclient -ljwt
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
jwt.h: Success
All tests succeeded.
```

### Troubleshooting

- If you encounter errors with the PostgreSQL connection, ensure the database and role exist and match your connection string.
- If the `postgres` role doesn't exist, create it using the instructions in step 5.

### License

This project is licensed under the [MIT License](LICENSE).
