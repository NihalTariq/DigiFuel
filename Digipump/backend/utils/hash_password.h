#ifndef HASH_PASSWORD_H
#define HASH_PASSWORD_H
#include <stddef.h>

int hashPassword(const char *password, char *hash, size_t hash_size);
int verifyPassword(const char *password,const char *storedHash
);

#endif