#include <string.h>
#include <microhttpd.h>
#include <stdio.h>

#define PORT 8080

// Function to handle incoming connections
static enum MHD_Result
answer_to_connection(void *cls, struct MHD_Connection *connection,
                     const char *url, const char *method,
                     const char *version, const char *upload_data,
                     size_t *upload_data_size, void **con_cls)
{
    const char *page = "<html><body>Hello, browser!</body></html>";
    struct MHD_Response *response;
    enum MHD_Result ret;

    // Create the HTTP response
    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);

    // Queue the response to be sent to the client
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);

    // Clean up the response object
    MHD_destroy_response(response);

    return ret;
}

int main(void)
{
    struct MHD_Daemon *daemon;

    // Start the daemon to listen on the specified port
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);

    // Check if the daemon started successfully
    if (NULL == daemon) {
        printf("Failed to start the daemon\n");
        return 1;
    }

    printf("Daemon started successfully on port %d\n", PORT);

    // Wait for a key press before shutting down the daemon
    getchar();

    // Stop the daemon
    MHD_stop_daemon(daemon);

    return 0;
}