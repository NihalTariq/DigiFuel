
#include <stdio.h>
#include <string.h>
#include <mysql.h>

#include "auth.h"
#include "../utils/hash_password.h"

static void setLoginError(
    char *error,
    size_t errorSize,
    const char *message
)
{
    if (error != NULL && errorSize > 0)
        snprintf(error, errorSize, "%s", message);
}

int login(MYSQL *conn,const LoginData *data,UserSession *session,char *error,size_t errorSize)
{
    MYSQL_STMT *stmt = NULL;

    int userID = 0;
    int success = 0;

    char storedHash[256] = {0};
    char role[21] = {0};

    unsigned long inputLength;
    unsigned long hashLength = 0;
    unsigned long roleLength = 0;

    if (session == NULL)
        return 0;

    session->userID = 0;
    session->role[0] = '\0';
    session->isLoggedIn = 0;

    setLoginError(error, errorSize,
                  "Invalid username/email or password.");

    if (conn == NULL || data == NULL)
        return 0;

    if (strlen(data->usernameEmail) == 0 ||
        strlen(data->usernameEmail) > 60 ||
        strlen(data->password) == 0 ||
        strlen(data->password) > 100)
        return 0;

    const char *query =
        "SELECT u_id, userpass, role "
        "FROM tbl_user "
        "WHERE username = ? OR email = ? "
        "LIMIT 1";

    stmt = mysql_stmt_init(conn);

    if (stmt == NULL)
        goto cleanup;

    if (mysql_stmt_prepare(stmt, query, strlen(query)))
        goto cleanup;

    inputLength =
        (unsigned long)strlen(data->usernameEmail);

    MYSQL_BIND paramBind[2] = {0};

    paramBind[0].buffer_type = MYSQL_TYPE_STRING;
    paramBind[0].buffer = (void *)data->usernameEmail;
    paramBind[0].buffer_length = inputLength;
    paramBind[0].length = &inputLength;

    paramBind[1].buffer_type = MYSQL_TYPE_STRING;
    paramBind[1].buffer = (void *)data->usernameEmail;
    paramBind[1].buffer_length = inputLength;
    paramBind[1].length = &inputLength;

    if (mysql_stmt_bind_param(stmt, paramBind) ||
        mysql_stmt_execute(stmt))
        goto cleanup;

    if (mysql_stmt_store_result(stmt))
        goto cleanup;

    MYSQL_BIND resultBind[3] = {0};

    resultBind[0].buffer_type = MYSQL_TYPE_LONG;
    resultBind[0].buffer = &userID;

    resultBind[1].buffer_type = MYSQL_TYPE_STRING;
    resultBind[1].buffer = storedHash;
    resultBind[1].buffer_length = sizeof(storedHash) - 1;
    resultBind[1].length = &hashLength;

    resultBind[2].buffer_type = MYSQL_TYPE_STRING;
    resultBind[2].buffer = role;
    resultBind[2].buffer_length = sizeof(role) - 1;
    resultBind[2].length = &roleLength;

    if (mysql_stmt_bind_result(stmt, resultBind))
        goto cleanup;

    if (mysql_stmt_fetch(stmt) != 0)
        goto cleanup;

    if (hashLength >= sizeof(storedHash) ||
        roleLength >= sizeof(role))
        goto cleanup;

    storedHash[hashLength] = '\0';
    role[roleLength] = '\0';

    mysql_stmt_close(stmt);
    stmt = NULL;


    if (!verifyPassword(data->password, storedHash))
        goto cleanup;

    session->userID = userID;

    snprintf(
        session->role,
        sizeof(session->role),
        "%s",
        role
    );

    session->isLoggedIn = 1;

    setLoginError(error, errorSize, "Login successful.");

    success = 1;

cleanup:

    if (stmt != NULL)
        mysql_stmt_close(stmt);

    memset(storedHash, 0, sizeof(storedHash));

    if (!success)
    {
        session->userID = 0;
        session->role[0] = '\0';
        session->isLoggedIn = 0;
    }

    return success;
}

//Logout Function
void logout(UserSession *session)
{
    if (session == NULL)
        return;

    session->userID = 0;
    session->role[0] = '\0';
    session->isLoggedIn = 0;
}