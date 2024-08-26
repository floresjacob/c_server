#include <stdio.h>            // Standard I/O functions
#include <string.h>           // String manipulation functions (e.g., strlen)
#include <microhttpd.h>       // MicroHTTPD for running a simple HTTP server
#include <sodium.h>           // Libsodium for cryptographic functions
#include <libpq-fe.h>         // Libpq for PostgreSQL client interactions
#include <mysql.h>            // MySQL client library
#include <jwt.h>              // JWT library for JSON Web Tokens

#define PORT 8080             // Define the port on which the HTTP server will run

// Simple request handler for MicroHTTPD
static enum MHD_Result
answer_to_connection(void *cls, struct MHD_Connection *connection,
                     const char *url, const char *method,
                     const char *version, const char *upload_data,
                     size_t *upload_data_size, void **con_cls)
{
    // HTML content to be served in response
    const char *page = "<html><body>Hello, browser!</body></html>";
    struct MHD_Response *response;  // HTTP response structure
    enum MHD_Result ret;            // Result of queuing the response

    // Create a response with the given HTML content
    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);

    // Queue the response to be sent to the client
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);

    // Clean up the response object
    MHD_destroy_response(response);

    return ret;  // Return the result of the HTTP response queueing
}

int main() {
    // Test libmicrohttpd by starting a daemon
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);

    // Check if the daemon started successfully
    if (NULL == daemon) {
        printf("Failed to start the daemon\n");
        return 1;  // Exit if the daemon failed to start
    }

    printf("Daemon started successfully on port %d\n", PORT);

    // Test libsodium by initializing it
    if (sodium_init() < 0) {
        printf("libsodium: Initialization failed\n");
        MHD_stop_daemon(daemon);  // Stop the daemon if libsodium initialization fails
        return 1;
    } else {
        printf("libsodium: Success\n");
    }

    // Perform password hashing using libsodium
    unsigned char hash[crypto_pwhash_STRBYTES];
    const char *password = "testpassword";
    if (crypto_pwhash_str((char *)hash, password, strlen(password),
                          crypto_pwhash_OPSLIMIT_MIN, crypto_pwhash_MEMLIMIT_MIN) != 0) {
        printf("libsodium: Password hashing failed\n");
        MHD_stop_daemon(daemon);  // Stop the daemon if password hashing fails
        return 1;
    } else {
        printf("libsodium: Password hashing success\n");
    }

    // Test libpq by connecting to the 'cserver' PostgreSQL database using the 'postgres' role
    PGconn *conn = PQconnectdb("user=postgres dbname=cserver host=/tmp");
    if (PQstatus(conn) != CONNECTION_OK) {
        printf("libpq: Connection failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        MHD_stop_daemon(daemon);  // Stop the daemon if the PostgreSQL connection fails
        return 1;
    } else {
        printf("libpq: Success\n");
        PQfinish(conn);  // Close the PostgreSQL connection
    }

    // Test mysql-client by initializing a MySQL connection
    MYSQL *mysql = mysql_init(NULL);
    if (mysql == NULL) {
        printf("mysql-client: Initialization failed\n");
        MHD_stop_daemon(daemon);  // Stop the daemon if MySQL initialization fails
        return 1;
    } else {
        printf("mysql-client: Success\n");
        mysql_close(mysql);  // Close the MySQL connection
    }

    // Test jwt.h
    jwt_t *jwt = NULL;
    if (jwt_new(&jwt) != 0) {
        printf("jwt.h: Initialization failed\n");
        MHD_stop_daemon(daemon);  // Stop the daemon if JWT initialization fails
        return 1;
    }

    // Set the algorithm for the JWT (for example, HS256 with no key)
    jwt_set_alg(jwt, JWT_ALG_NONE, NULL, 0);

    printf("jwt.h: Success\n");

    // Free the JWT object
    jwt_free(jwt);

    printf("All tests succeeded.\n");

    // Wait for a key press before shutting down the daemon
    getchar();

    // Stop the HTTP daemon before exiting
    MHD_stop_daemon(daemon);

    return 0;  // Exit the program successfully
}