#ifndef UTILS_H
#define UTILS_H

char* hash_password(const char* password);
int verify_password(const char* password, const char* hash);
char *extract_value(struct MHD_Connection *connection, const char *key); // Declaration only
enum MHD_Result send_response(struct MHD_Connection *connection, unsigned int status_code, const char *text);

#endif // UTILS_H