#include <microhttpd.h>   // MicroHTTPD library for creating an HTTP server
#include <sodium.h>       // Libsodium for cryptographic functions, including password hashing
#include <stdio.h>        // Standard input/output functions
#include <stdlib.h>       // Standard library functions like memory management
#include "routes.h" // Header file for route handling functions

#define PORT 8080  // Define the port number on which the server will listen

int main() {
    // Initialize libsodium, which is necessary for secure password hashing and other cryptographic operations
    if (sodium_init() < 0) {
        // If initialization fails, print an error message and exit with a non-zero status
        fprintf(stderr, "Failed to initialize libsodium\n");
        return 1;
    }

    // Start the libmicrohttpd daemon with an internal polling thread
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &handle_request, NULL, MHD_OPTION_END);

    // Check if the daemon started successfully
    if (NULL == daemon) {
        // If the daemon fails to start, print an error message and exit with a non-zero status
        fprintf(stderr, "Failed to start the server daemon\n");
        return 1;
    }

    // Print a success message indicating the server is running and on which port
    printf("Daemon started successfully on port %d\n", PORT);

    // Keep the server running indefinitely
    getchar();  // Waits for the user to press Enter, keeping the server running

    // Stop the daemon and clean up resources when the program ends
    MHD_stop_daemon(daemon);

    return 0;  // Exit the program with a status of 0 (indicating success)
}