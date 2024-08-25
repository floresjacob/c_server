#include <jwt.h>          // Include the libjwt library for JWT functions
#include <stdlib.h>       // Standard library for memory management
#include <string.h>       // String manipulation functions
#include <time.h>         // Time functions for setting expiration
#include "project_jwt.h"  // Your custom header for JWT-related declarations
#include <stdbool.h>      // Include this to use bool, true, and false
#include <microhttpd.h>   // Include for MHD_Connection and MHD functions

// Function to generate a JWT for a user
char* generate_jwt(const char* username) {
    jwt_t *jwt = NULL;
    char *token = NULL;

    // Create a new JWT object
    if (jwt_new(&jwt) != 0) {
        return NULL;
    }

    // Set claims (e.g., subject and expiration time)
    jwt_add_grant(jwt, "sub", username);
    jwt_add_grant_int(jwt, "exp", time(NULL) + 3600); // Token expires in 1 hour

    // Encode the JWT
    token = jwt_encode_str(jwt);

    // Free the JWT object
    jwt_free(jwt);

    return token;
}

// Function to verify a JWT and return whether it's valid
bool verify_jwt(const char* token, char** username) {
    jwt_t *jwt = NULL;

    // Decode the JWT token
    if (jwt_decode(&jwt, token, NULL, 0) != 0) {
        return false;
    }

    // Validate expiration time
    if (time(NULL) > jwt_get_grant_int(jwt, "exp")) {
        jwt_free(jwt);
        return false;
    }

    // Retrieve the username from the token's subject claim
    const char *sub = jwt_get_grant(jwt, "sub");
    if (sub) {
        *username = strdup(sub);
    }

    // Free the JWT object
    jwt_free(jwt);

    return true;
}

// Function to validate JWT in a middleware context
bool validate_jwt_middleware(struct MHD_Connection *connection, char **username) {
    // Extract the Authorization header from the request
    const char *auth_header = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Authorization");
    
    if (auth_header == NULL || strncmp(auth_header, "Bearer ", 7) != 0) {
        // If no Authorization header or it doesn't start with "Bearer ", return false
        return false;
    }

    // Extract the token part from the "Bearer " prefix
    const char *token = auth_header + 7;

    // Verify the JWT token
    if (!verify_jwt(token, username)) {
        // If verification fails, return false
        return false;
    }

    // If the token is valid, return true
    return true;
}