#ifndef DB_H
#define DB_H

#include <libpq-fe.h>  // Include libpq headers for PostgreSQL

PGconn *connect_db();
int store_user(const char *username, const char *email, const char *hashed_password);
char *get_hashed_password_from_db(const char *username);
#endif // DB_H