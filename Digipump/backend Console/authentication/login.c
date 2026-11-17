
#include <stdio.h>
#include <string.h>
#include <mysql.h>

#include "auth.h"
#include "../utils/hash_password.h"

int login(MYSQL *conn, UserSession *session)

{
    //Session Initialization
    session->userID = 0;
    session->role[0] = '\0';
    session->isLoggedIn = 0;

    char username_email[61];
    char password[101];
    char storedHash[256];
    char role[21];

    int userID;

    MYSQL_STMT *stmt;
    MYSQL_BIND paramBind[2];
    MYSQL_BIND resultBind[3];

    unsigned long inputLength;
    unsigned long hashLength;
    unsigned long roleLength;

    printf("\n========== DIGI FUELS LOGIN ==========\n");
    //Input Credentials
    printf("Username or Email: ");
    scanf(" %60s", username_email);

    printf("Password: ");
    scanf(" %100s", password);

    const char *query =
        "SELECT u_id, userpass, role "
        "FROM tbl_user "
        "WHERE username = ? OR email = ? "
        "LIMIT 1";

    stmt = mysql_stmt_init(conn);

    if (stmt == NULL)
    {
        printf("Statement initialization failed!\n");
        memset(password, 0, sizeof(password));
        return 0;
    }

    if (mysql_stmt_prepare(stmt, query, strlen(query)))
    {
        printf("Query preparation failed: %s\n",
               mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        memset(password, 0, sizeof(password));
        return 0;
    }

    inputLength = strlen(username_email);

    memset(paramBind, 0, sizeof(paramBind));

    paramBind[0].buffer_type = MYSQL_TYPE_STRING;
    paramBind[0].buffer = username_email;
    paramBind[0].buffer_length = sizeof(username_email);
    paramBind[0].length = &inputLength;

    paramBind[1].buffer_type = MYSQL_TYPE_STRING;
    paramBind[1].buffer = username_email;
    paramBind[1].buffer_length = sizeof(username_email );
    paramBind[1].length = &inputLength;

    if (mysql_stmt_bind_param(stmt, paramBind) ||
        mysql_stmt_execute(stmt))
    {
        printf("Login search failed: %s\n",
               mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        memset(password, 0, sizeof(password));
        return 0;
    }

    if (mysql_stmt_store_result(stmt))
    {
        printf("Result retrieval failed: %s\n",
               mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        memset(password, 0, sizeof(password));
        return 0;
    }

    memset(resultBind, 0, sizeof(resultBind));

    resultBind[0].buffer_type = MYSQL_TYPE_LONG;
    resultBind[0].buffer = &userID;

    resultBind[1].buffer_type = MYSQL_TYPE_STRING;
    resultBind[1].buffer = storedHash;
    resultBind[1].buffer_length = sizeof(storedHash);
    resultBind[1].length = &hashLength;

    resultBind[2].buffer_type = MYSQL_TYPE_STRING;
    resultBind[2].buffer = role;
    resultBind[2].buffer_length = sizeof(role);
    resultBind[2].length = &roleLength;

    if (mysql_stmt_bind_result(stmt, resultBind))
    {
        printf("Result binding failed: %s\n",
        mysql_stmt_error(stmt));     

        mysql_stmt_close(stmt);
        memset(password, 0, sizeof(password));
        return 0;
    }

    int fetchStatus = mysql_stmt_fetch(stmt);

    if (fetchStatus != 0)
    {
        printf("Invalid Credentials.\n");

        mysql_stmt_close(stmt);
        memset(password, 0, sizeof(password));
        return 0;
    }

    storedHash[hashLength < sizeof(storedHash) ?
               hashLength : sizeof(storedHash) - 1] = '\0';

    role[roleLength < sizeof(role) ?
         roleLength : sizeof(role) - 1] = '\0';

    mysql_stmt_close(stmt);

    // Verify entered password
    if (!verifyPassword(password, storedHash))
    {
        printf("Invalid username/email or password.\n");

        memset(password, 0, sizeof(password));
        return 0;
    }

    memset(password, 0, sizeof(password));

    //Session state updated
    session->userID = userID;

    strcpy(session->role, role);
    session->isLoggedIn = 1;

    printf("\nLogin successful!\n");
    printf("Welcome!\n");
    printf("User ID: %d\n", userID);
    printf("Role: %s\n", role);

    return 1;
}

//Logout Function
void logout(UserSession *session)
{
    //Destroys session
    session->userID = 0;
    session->role[0] = '\0';
    session->isLoggedIn = 0;

    printf("\nYou have been logged out successfully!\n");
}