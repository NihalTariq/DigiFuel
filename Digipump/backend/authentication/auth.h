
#ifndef AUTH_H
#define AUTH_H

#include <mysql.h>
#include <stddef.h>

typedef struct
{
    int userID;
    char role[21];
    int isLoggedIn;
} UserSession;

typedef struct
{
    char username[61];
    char email[61];
    char password[101];
    char firstName[61];
    char lastName[61];
    char phone[21];
} SignupData;

typedef struct
{
    char usernameEmail[61];
    char password[101];
} LoginData;

int signup(MYSQL *conn, const SignupData *data, int *newUserID,
           char *error, size_t errorSize);

int login(MYSQL *conn, const LoginData *data, UserSession *session,
          char *error, size_t errorSize);

void logout(UserSession *session);

#endif
