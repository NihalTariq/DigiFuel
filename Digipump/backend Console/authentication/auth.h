
#ifndef AUTH_H
#define AUTH_H

#include <mysql.h>

typedef struct
{
    int userID;
    char role[21];
    int isLoggedIn;
} UserSession;

int signup(MYSQL *conn);

int login(MYSQL *conn, UserSession *session);

void logout(UserSession *session);

#endif