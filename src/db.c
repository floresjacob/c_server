#include <stdio.h>         // Standard input/output library
#include <stdlib.h>        // Standard library for memory allocation, process control, etc.
#include <string.h>        // String manipulation functions
#include "db.h"    // Header file for database-related functions
#include <libpq-fe.h>      // PostgreSQL library for client-side API

// Function to connect to the PostgreSQL database
PGconn *connect_db() {
    // Connection string with database name, user, password, host address, and port
    const char *conninfo = "dbname=c_server user=postgres password=password hostaddr=127.0.0.1 port=5432";
    
    // Establishing a connection to the PostgreSQL database
    PGconn *conn = PQconnectdb(conninfo);

    // Check if the connection was successful
    if (PQstatus(conn) != CONNECTION_OK) {
        // If the connection failed, print the error message
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        // Close the connection and return NULL to indicate failure
        PQfinish(conn);
        return NULL;
    }

    // Return the connection object if successful
    return conn;
}

// Function to store a new user in the database
int store_user(const char *username, const char *email, const char *hashed_password) {
    // Connect to the database
    PGconn *conn = connect_db();
    if (conn == NULL) {
        // If the connection failed, return 0 to indicate failure
        return 0;
    }

    // Array of parameter values to be used in the SQL query
    const char *paramValues[3] = {username, email, hashed_password};
    // SQL query with placeholders for parameters
    const char *query = "INSERT INTO users (username, email, password_hash) VALUES ($1, $2, $3)";
    
    // Execute the SQL query with the provided parameters
    PGresult *res = PQexecParams(conn, query, 3, NULL, paramValues, NULL, NULL, 0);
    
    // Check if the SQL command was executed successfully
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        // If the command failed, print the error message
        fprintf(stderr, "INSERT command failed: %s", PQerrorMessage(conn));
        // Clear the result object and close the connection
        PQclear(res);
        PQfinish(conn);
        // Return 0 to indicate failure
        return 0;
    }

    // Clear the result object to free memory
    PQclear(res);
    // Close the database connection
    PQfinish(conn);
    // Return 1 to indicate success
    return 1;
}

// Fetch the hashed password for a user from the database
char *get_hashed_password_from_db(const char *username) {
    PGconn *conn = connect_db();
    if (!conn) return NULL;

    const char *paramValues[1] = { username };
    PGresult *res = PQexecParams(conn,
                                 "SELECT password_hash FROM users WHERE username=$1",
                                 1,
                                 NULL,
                                 paramValues,
                                 NULL,
                                 NULL,
                                 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        PQfinish(conn);
        return NULL;
    }

    char *password = strdup(PQgetvalue(res, 0, 0));

    PQclear(res);
    PQfinish(conn);

    return password;
}