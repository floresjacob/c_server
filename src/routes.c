#include <microhttpd.h>   // MicroHTTPD library for creating an HTTP server
#include <stdlib.h>       // Standard library for memory management and general utilities
#include <string.h>       // String manipulation functions
#include "db.h"           // Header file for database-related functions
#include "utils.h"        // Header file for utility functions like password hashing
#include "project_jwt.h"  // Header file for JWT operations
#include <stdbool.h>

#define POSTBUFFERSIZE 512  // Defines the buffer size for handling POST data

bool validate_jwt_middleware(struct MHD_Connection *connection, char **username);

struct connection_info_struct {
    char *username;
    char *email;
    char *password;
    struct MHD_PostProcessor *postprocessor;
};

// Correct the function signature to match `MHD_PostDataIterator`'s expected return type.
static enum MHD_Result iterate_post(void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
                                    const char *filename, const char *content_type, const char *transfer_encoding,
                                    const char *data, uint64_t off, size_t size) {
    struct connection_info_struct *con_info = coninfo_cls;

    if (0 == strcmp(key, "username")) {
        con_info->username = strndup(data, size);
    } else if (0 == strcmp(key, "email")) {
        con_info->email = strndup(data, size);
    } else if (0 == strcmp(key, "password")) {
        con_info->password = strndup(data, size);
    }

    return MHD_YES;
}

// Corrected handle_request function signature
enum MHD_Result handle_request(void *cls,
                               struct MHD_Connection *connection,
                               const char *url,
                               const char *method,
                               const char *version,
                               const char *upload_data,
                               size_t *upload_data_size,
                               void **con_cls) {
    if (NULL == *con_cls) {
        struct connection_info_struct *con_info = malloc(sizeof(struct connection_info_struct));
        con_info->username = NULL;
        con_info->email = NULL;
        con_info->password = NULL;

        con_info->postprocessor = MHD_create_post_processor(connection, POSTBUFFERSIZE, iterate_post, (void *)con_info);

        *con_cls = (void *)con_info;

        return MHD_YES;
    }

    if (0 == strcmp(method, "POST")) {
        struct connection_info_struct *con_info = *con_cls;

        if (*upload_data_size != 0) {
            MHD_post_process(con_info->postprocessor, upload_data, *upload_data_size);
            *upload_data_size = 0;
            return MHD_YES;
        } else if (0 == strcmp(url, "/register")) {
            printf("Username: %s\n", con_info->username);
            printf("Email: %s\n", con_info->email);
            printf("Password: %s\n", con_info->password);

            if (con_info->username == NULL || con_info->email == NULL || con_info->password == NULL) {
                return send_response(connection, MHD_HTTP_BAD_REQUEST, "Missing fields");
            }

            char *hashed_password = hash_password(con_info->password);
            if (hashed_password == NULL) {
                return send_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, "Error hashing password");
            }

            if (!store_user(con_info->username, con_info->email, hashed_password)) {
                return send_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, "Database error");
            }

            free(hashed_password);
            return send_response(connection, MHD_HTTP_CREATED, "User registered successfully");
        }
    }

    if (0 == strcmp(url, "/login") && 0 == strcmp(method, "POST")) {
        struct connection_info_struct *con_info = *con_cls;

        printf("Username: %s\n", con_info->username);
        printf("Password: %s\n", con_info->password);

        if (con_info->username == NULL || con_info->password == NULL) {
            return send_response(connection, MHD_HTTP_BAD_REQUEST, "Missing fields");
        }

        // Retrieve the hashed password from the database based on the provided username
        char *stored_hashed_password = get_hashed_password_from_db(con_info->username);
        if (stored_hashed_password == NULL) {
            printf("No stored hashed password found for user: %s\n", con_info->username);
            return send_response(connection, MHD_HTTP_UNAUTHORIZED, "Invalid username or password");
        }

        // Log the stored hashed password
        printf("Stored hashed password: %s\n", stored_hashed_password);

        // Compare the provided password with the stored hashed password
        // Let's add more detailed logging here:
        if (verify_password(con_info->password, stored_hashed_password)) {
            printf("Password verification succeeded for user: %s\n", con_info->username);

            char *jwt = generate_jwt(con_info->username);
            if (jwt == NULL) {
                free(stored_hashed_password);
                return send_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, "Error generating token");
            }

            free(stored_hashed_password);
            return send_response(connection, MHD_HTTP_OK, jwt);
        } else {
            printf("Password verification failed for user: %s\n", con_info->username);
            free(stored_hashed_password);
            return send_response(connection, MHD_HTTP_UNAUTHORIZED, "Invalid username or password");
        }

        // Generate a JWT token for the authenticated user
        char *jwt = generate_jwt(con_info->username);
        if (jwt == NULL) {
            free(stored_hashed_password);
            return send_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, "Error generating token");
        }

        free(stored_hashed_password);
        return send_response(connection, MHD_HTTP_OK, jwt);
    }

    if (0 == strcmp(url, "/protected") && 0 == strcmp(method, "GET")) {
        char *username = NULL;

        // Validate JWT
        if (!validate_jwt_middleware(connection, &username)) {
            return send_response(connection, MHD_HTTP_UNAUTHORIZED, "Invalid or missing token");
        }

        // Create the response content
        char response[512];
        snprintf(response, sizeof(response), "Hello, %s! You have accessed a protected route.", username);

        // Log the response content for debugging
        printf("Sending response: %s\n", response);

        // Send the response before freeing username
        enum MHD_Result ret = send_response(connection, MHD_HTTP_OK, response);

        // Now it's safe to free the memory allocated for the username
        free(username);

        return ret;
    }

    return send_response(connection, MHD_HTTP_NOT_FOUND, "Endpoint not found");
}