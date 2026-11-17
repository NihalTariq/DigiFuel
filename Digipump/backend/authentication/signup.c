
#include <stdio.h>
#include <string.h>
#include <mysql.h>

#include "auth.h"
#include "../utils/hash_password.h"

static void setSignupError(
    char *error,
    size_t errorSize,
    const char *message
)
{
    if (error != NULL && errorSize > 0)
        snprintf(error, errorSize, "%s", message);
}

int signup(MYSQL *conn,const SignupData *data,int *newUserID,char *error,size_t errorSize)
{
    MYSQL_STMT *stmt = NULL;

    int transactionStarted = 0;
    int success = 0;
    int userID = 0;

    char hashedPassword[150] = {0};

    const char *role = "Citizen";

    if (newUserID != NULL)
        *newUserID = 0;

    setSignupError(error, errorSize,
                   "Invalid registration data.");

    if (conn == NULL || data == NULL || newUserID == NULL)
        return 0;

    /* Validate input lengths */

    if (strlen(data->username) == 0 ||
        strlen(data->username) > 60 ||
        strlen(data->email) == 0 ||
        strlen(data->email) > 60 ||
        strlen(data->password) == 0 ||
        strlen(data->password) > 100 ||
        strlen(data->firstName) == 0 ||
        strlen(data->firstName) > 60 ||
        strlen(data->lastName) == 0 ||
        strlen(data->lastName) > 60 ||
        strlen(data->phone) == 0 ||
        strlen(data->phone) > 20)
    {
        setSignupError(error, errorSize,
                       "One or more fields are invalid.");
        return 0;
    }

    /* Basic email validation */

    const char *at = strchr(data->email, '@');

    if (at == NULL || at == data->email ||
        strchr(at, '.') == NULL)
    {
        setSignupError(error, errorSize,
                       "Invalid email address.");
        return 0;
    }

    /* Hash password */

    if (!hashPassword(
            data->password,
            hashedPassword,
            sizeof(hashedPassword)))
    {
        setSignupError(error, errorSize,
                       "Password hashing failed.");
        return 0;
    }

    /* Start transaction */

    if (mysql_query(conn, "START TRANSACTION"))
    {
        setSignupError(error, errorSize,
                       "Database transaction failed.");
        goto cleanup;
    }

    transactionStarted = 1;

    /* Insert user */

    const char *userSQL =
        "INSERT INTO tbl_user "
        "(username, email, userpass, role) "
        "VALUES (?, ?, ?, ?)";

    stmt = mysql_stmt_init(conn);

    if (stmt == NULL)
    {
        setSignupError(error, errorSize,
                       "Statement initialization failed.");
        goto cleanup;
    }

    if (mysql_stmt_prepare(stmt, userSQL, strlen(userSQL)))
    {
        setSignupError(error, errorSize,
                       "User query preparation failed.");
        goto cleanup;
    }

    MYSQL_BIND bind[4] = {0};

    unsigned long usernameLength =
        (unsigned long)strlen(data->username);

    unsigned long emailLength =
        (unsigned long)strlen(data->email);

    unsigned long passwordLength =
        (unsigned long)strlen(hashedPassword);

    unsigned long roleLength =
        (unsigned long)strlen(role);

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (void *)data->username;
    bind[0].buffer_length = usernameLength;
    bind[0].length = &usernameLength;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (void *)data->email;
    bind[1].buffer_length = emailLength;
    bind[1].length = &emailLength;

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = hashedPassword;
    bind[2].buffer_length = passwordLength;
    bind[2].length = &passwordLength;

    bind[3].buffer_type = MYSQL_TYPE_STRING;
    bind[3].buffer = (void *)role;
    bind[3].buffer_length = roleLength;
    bind[3].length = &roleLength;

    if (mysql_stmt_bind_param(stmt, bind) ||
        mysql_stmt_execute(stmt))
    {
        if (mysql_stmt_errno(stmt) == 1062)
        {
            setSignupError(error, errorSize,
                           "Username or email already exists.");
        }
        else
        {
            setSignupError(error, errorSize,
                           "User registration failed.");
        }

        goto cleanup;
    }

    userID = (int)mysql_insert_id(conn);

    mysql_stmt_close(stmt);
    stmt = NULL;

    /* Insert citizen profile */

    const char *citizenSQL =
        "INSERT INTO tbl_citizen "
        "(f_name, l_name, phone, u_id_fk) "
        "VALUES (?, ?, ?, ?)";

    stmt = mysql_stmt_init(conn);

    if (stmt == NULL)
    {
        setSignupError(error, errorSize,
                       "Citizen statement initialization failed.");
        goto cleanup;
    }

    if (mysql_stmt_prepare(
            stmt, citizenSQL, strlen(citizenSQL)))
    {
        setSignupError(error, errorSize,
                       "Citizen query preparation failed.");
        goto cleanup;
    }

    MYSQL_BIND citizenBind[4] = {0};

    unsigned long firstLength =
        (unsigned long)strlen(data->firstName);

    unsigned long lastLength =
        (unsigned long)strlen(data->lastName);

    unsigned long phoneLength =
        (unsigned long)strlen(data->phone);

    citizenBind[0].buffer_type = MYSQL_TYPE_STRING;
    citizenBind[0].buffer = (void *)data->firstName;
    citizenBind[0].buffer_length = firstLength;
    citizenBind[0].length = &firstLength;

    citizenBind[1].buffer_type = MYSQL_TYPE_STRING;
    citizenBind[1].buffer = (void *)data->lastName;
    citizenBind[1].buffer_length = lastLength;
    citizenBind[1].length = &lastLength;

    citizenBind[2].buffer_type = MYSQL_TYPE_STRING;
    citizenBind[2].buffer = (void *)data->phone;
    citizenBind[2].buffer_length = phoneLength;
    citizenBind[2].length = &phoneLength;

    citizenBind[3].buffer_type = MYSQL_TYPE_LONG;
    citizenBind[3].buffer = &userID;

    if (mysql_stmt_bind_param(stmt, citizenBind) ||
        mysql_stmt_execute(stmt))
    {
        setSignupError(error, errorSize,
                       "Citizen profile registration failed.");
        goto cleanup;
    }

    mysql_stmt_close(stmt);
    stmt = NULL;

    /* Commit transaction */

    if (mysql_query(conn, "COMMIT"))
    {
        setSignupError(error, errorSize,
                       "Registration commit failed.");
        goto cleanup;
    }

    transactionStarted = 0;

    *newUserID = userID;

    setSignupError(error, errorSize,
                   "Citizen registered successfully.");

    success = 1;

cleanup:

    if (stmt != NULL)
        mysql_stmt_close(stmt);

    if (transactionStarted)
        mysql_query(conn, "ROLLBACK");

    memset(hashedPassword, 0, sizeof(hashedPassword));

    return success;
}