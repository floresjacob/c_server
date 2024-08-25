#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include <microhttpd.h>
#include "utils.h"

#define HASH_STR_LEN crypto_pwhash_STRBYTES

char* hash_password(const char* password) {
    char hash[HASH_STR_LEN];

    if (crypto_pwhash_str(hash, password, strlen(password),
                          crypto_pwhash_OPSLIMIT_INTERACTIVE,
                          crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        // Out of memory or another error
        return NULL;
    }

    return strdup(hash);
}

int verify_password(const char* password, const char* hash) {
    int result = crypto_pwhash_str_verify(hash, password, strlen(password));

    if (result == 0) {
        printf("Password verification succeeded.\n");
    } else {
        printf("Password verification failed.\n");
    }

    return result == 0;
}

// Extracts a value from the POST data
char *extract_value(struct MHD_Connection *connection, const char *key) {
    const char *value = MHD_lookup_connection_value(connection, MHD_POSTDATA_KIND, key);
    if (value == NULL) {
        return NULL;
    }
    return strdup(value);
}

// Sends an HTTP response
enum MHD_Result send_response(struct MHD_Connection *connection, unsigned int status_code, const char *text) {
    struct MHD_Response *response;
    enum MHD_Result ret;

    // Debugging: print the text to be sent
    printf("Sending response: %s\n", text);

    // Create a response object
    response = MHD_create_response_from_buffer(strlen(text), (void *) text, MHD_RESPMEM_MUST_COPY);
    if (!response) {
        fprintf(stderr, "Failed to create response from buffer\n");
        return MHD_NO;
    }

    // Add content type header for plain text
    MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, "text/plain; charset=utf-8");

    // Queue the response
    ret = MHD_queue_response(connection, status_code, response);
    if (ret != MHD_YES) {
        fprintf(stderr, "Failed to queue response\n");
    }

    // Destroy the response object after sending
    MHD_destroy_response(response);

    return ret;
}
