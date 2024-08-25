#ifndef JWT_H
#define JWT_H

#include <stdbool.h>  // For using bool type

// Declare your custom functions for generating and verifying JWTs
char* generate_jwt(const char* username);
bool verify_jwt(const char* token, char** username);
bool validate_jwt_middleware(struct MHD_Connection *connection, char **username);

#endif // JWT_H