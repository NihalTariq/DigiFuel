
#include <stdio.h>
#include <string.h>
#include <mysql.h>

#include "admin.h"
#include "../utils/hash_password.h"

int add_admin(MYSQL *conn)
{
    char username[61];
    char email[61];
    char password[101];
    char hashedPassword[150];

    int userID;

    MYSQL_STMT *stmt;
    MYSQL_BIND bind[4];

    unsigned long usernameLength;
    unsigned long emailLength;
    unsigned long passwordLength;
    unsigned long roleLength;

    const char *role = "Admin";

    printf("\n===== ADD ADMIN =====\n");

    printf("Username: ");
    scanf(" %60s", username);

    printf("Email: ");
    scanf(" %60s", email);

    printf("Password: ");
    scanf(" %100s", password);

    // Checks if  username or email already taken
    const char *checkSQL =
        "SELECT username, email FROM tbl_user "
        "WHERE username = ? OR email = ?";

    stmt = mysql_stmt_init(conn);

    if (stmt == NULL)
    {
        printf("Unable to initialize database statement.\n");
        memset(password, 0, sizeof(password));
        return 0;
    }

    if (mysql_stmt_prepare(stmt, checkSQL, strlen(checkSQL)))
    {
        printf("Database error: Unable to prepare validation query.\n");
        mysql_stmt_close(stmt);
        memset(password, 0, sizeof(password));
        return 0;
    }

    MYSQL_BIND checkBind[2];
    memset(checkBind, 0, sizeof(checkBind));

    checkBind[0].buffer_type = MYSQL_TYPE_STRING;
    checkBind[0].buffer = username;
    checkBind[0].buffer_length = sizeof(username);
    checkBind[0].length = &usernameLength;

    checkBind[1].buffer_type = MYSQL_TYPE_STRING;
    checkBind[1].buffer = email;
    checkBind[1].buffer_length = sizeof(email);
    checkBind[1].length = &emailLength;

    usernameLength = strlen(username);
    emailLength = strlen(email);

    if (mysql_stmt_bind_param(stmt, checkBind) ||
        mysql_stmt_execute(stmt))
    {
        printf("Database error: Unable to check account details.\n");
        mysql_stmt_close(stmt);
        memset(password, 0, sizeof(password));
        return 0;
    }

    mysql_stmt_store_result(stmt);

    if (mysql_stmt_num_rows(stmt) > 0)
    {
        printf("\nUsername/Email is already taken\n");

        mysql_stmt_close(stmt);
        memset(password, 0, sizeof(password));
        return 0;
    }

    mysql_stmt_close(stmt);

    // Generate password hash
    if (!hashPassword(password, hashedPassword,
                      sizeof(hashedPassword)))
    {
        printf("Password hashing failed!\n");
        memset(password, 0, sizeof(password));
        return 0;
    }

    memset(password, 0, sizeof(password));

    // Initialize insertion statement
    stmt = mysql_stmt_init(conn);

    if (stmt == NULL)
    {
        printf("Unable to initialize database statement.\n");
        return 0;
    }

    const char *userSQL =
        "INSERT INTO tbl_user "
        "(username, email, userpass, role) "
        "VALUES (?, ?, ?, ?)";

    if (mysql_stmt_prepare(stmt, userSQL, strlen(userSQL)))
    {
        printf("Database error: Unable to prepare admin insertion.\n");
        mysql_stmt_close(stmt);
        return 0;
    }

    memset(bind, 0, sizeof(bind));

    usernameLength = strlen(username);
    emailLength = strlen(email);
    passwordLength = strlen(hashedPassword);
    roleLength = strlen(role);

    // Username
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = username;
    bind[0].buffer_length = sizeof(username);
    bind[0].length = &usernameLength;

    // Email
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = email;
    bind[1].buffer_length = sizeof(email);
    bind[1].length = &emailLength;

    // Password hash
    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = hashedPassword;
    bind[2].buffer_length = sizeof(hashedPassword);
    bind[2].length = &passwordLength;

    // Role
    bind[3].buffer_type = MYSQL_TYPE_STRING;
    bind[3].buffer = (char *)role;
    bind[3].buffer_length = roleLength;
    bind[3].length = &roleLength;

    if (mysql_stmt_bind_param(stmt, bind) ||
        mysql_stmt_execute(stmt))
    {
        unsigned int errorCode = mysql_stmt_errno(stmt);

        if (errorCode == 1062)
        {
            printf("Username or email is already taken.\n");
        }
        else
        {
            printf("Admin insertion failed. Database error: %s\n",
                   mysql_stmt_error(stmt));
        }

        mysql_stmt_close(stmt);
        memset(hashedPassword, 0, sizeof(hashedPassword));
        return 0;
    }

    userID = (int)mysql_insert_id(conn);

    mysql_stmt_close(stmt);

    memset(hashedPassword, 0, sizeof(hashedPassword));

    printf("\nAdmin registered successfully!\n");
    printf("User ID: %d\n", userID);

    return 1;
}
