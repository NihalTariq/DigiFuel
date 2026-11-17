
#include <stdio.h>
#include <string.h>
#include <mysql.h>

#include "owner.h"
#include "../utils/hash_password.h"

int add_owner(MYSQL *conn)
{
    char username[61];
    char email[61];
    char password[101];
    char hashedPassword[150];

    char firstName[61];
    char lastName[61];
    char phone[21];

    int userID;

    MYSQL_STMT *stmt;
    MYSQL_BIND bind[4];

    unsigned long usernameLength;
    unsigned long emailLength;
    unsigned long passwordLength;
    unsigned long roleLength;

    const char *role = "Owner";

    printf("\n===== ADD PUMP OWNER =====\n");

    //Login Info
    printf("Username: ");
    scanf(" %60s", username);

    printf("Email: ");
    scanf(" %60s", email);

    printf("Password: ");
    scanf(" %100s", password);

    //Owner Info
    printf("First name: ");
    scanf(" %60s", firstName);

    printf("Last name: ");
    scanf(" %60s", lastName);

    printf("Phone: ");
    scanf(" %20s", phone);


    //Check if username or email already taken

    const char *checkSQL =
        "SELECT u_id FROM tbl_user "
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
        printf("Unable to prepare validation query: %s\n",
               mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        memset(password, 0, sizeof(password));
        return 0;
    }

    MYSQL_BIND checkBind[2];
    memset(checkBind, 0, sizeof(checkBind));

    usernameLength = strlen(username);
    emailLength = strlen(email);

    checkBind[0].buffer_type = MYSQL_TYPE_STRING;
    checkBind[0].buffer = username;
    checkBind[0].buffer_length = sizeof(username);
    checkBind[0].length = &usernameLength;

    checkBind[1].buffer_type = MYSQL_TYPE_STRING;
    checkBind[1].buffer = email;
    checkBind[1].buffer_length = sizeof(email);
    checkBind[1].length = &emailLength;

    if (mysql_stmt_bind_param(stmt, checkBind) ||
        mysql_stmt_execute(stmt))
    {
        printf("Unable to check account details: %s\n",
               mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        memset(password, 0, sizeof(password));
        return 0;
    }

    if (mysql_stmt_store_result(stmt))
    {
        printf("Unable to retrieve validation results: %s\n",
               mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        memset(password, 0, sizeof(password));
        return 0;
    }

    if (mysql_stmt_num_rows(stmt) > 0)
    {
        printf("\nUsername or email is already registered!\n");
        printf("Please use different details.\n");

        mysql_stmt_close(stmt);
        memset(password, 0, sizeof(password));
        return 0;
    }

    mysql_stmt_close(stmt);


    //Generate password hash

    if (!hashPassword(password, hashedPassword,
                      sizeof(hashedPassword)))
    {
        printf("Password hashing failed!\n");
        memset(password, 0, sizeof(password));
        return 0;
    }

    //Clear temporary password
    memset(password, 0, sizeof(password));



    if (mysql_query(conn, "START TRANSACTION"))
    {
        printf("Transaction failed: %s\n", mysql_error(conn));
        memset(hashedPassword, 0, sizeof(hashedPassword));
        return 0;
    }


    //Insert login info into tbl_user

    stmt = mysql_stmt_init(conn);

    if (stmt == NULL)
    {
        printf("Unable to initialize database statement.\n");
        mysql_query(conn, "ROLLBACK");
        memset(hashedPassword, 0, sizeof(hashedPassword));
        return 0;
    }

    const char *userSQL =
        "INSERT INTO tbl_user "
        "(username, email, userpass, role) "
        "VALUES (?, ?, ?, ?)";

    if (mysql_stmt_prepare(stmt, userSQL, strlen(userSQL)))
    {
        printf("User prepare failed: %s\n",
               mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        mysql_query(conn, "ROLLBACK");
        memset(hashedPassword, 0, sizeof(hashedPassword));
        return 0;
    }

    memset(bind, 0, sizeof(bind));

    usernameLength = strlen(username);
    emailLength = strlen(email);
    passwordLength = strlen(hashedPassword);
    roleLength = strlen(role);

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = username;
    bind[0].buffer_length = sizeof(username);
    bind[0].length = &usernameLength;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = email;
    bind[1].buffer_length = sizeof(email);
    bind[1].length = &emailLength;

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = hashedPassword;
    bind[2].buffer_length = sizeof(hashedPassword);
    bind[2].length = &passwordLength;

    bind[3].buffer_type = MYSQL_TYPE_STRING;
    bind[3].buffer = (char *)role;
    bind[3].buffer_length = roleLength;
    bind[3].length = &roleLength;

    if (mysql_stmt_bind_param(stmt, bind) ||
        mysql_stmt_execute(stmt))
    {
        if (mysql_stmt_errno(stmt) == 1062)
        {
            printf("Username or email is already taken!\n");
        }
        else
        {
            printf("User insertion failed: %s\n",
                   mysql_stmt_error(stmt));
        }

        mysql_stmt_close(stmt);
        mysql_query(conn, "ROLLBACK");
        memset(hashedPassword, 0, sizeof(hashedPassword));
        return 0;
    }

    //Newly generated User ID
    userID = (int)mysql_insert_id(conn);

    mysql_stmt_close(stmt);


    //Insert owner profile into tbl_owners

    stmt = mysql_stmt_init(conn);

    if (stmt == NULL)
    {
        printf("Unable to initialize owner statement.\n");
        mysql_query(conn, "ROLLBACK");
        memset(hashedPassword, 0, sizeof(hashedPassword));
        return 0;
    }

    const char *ownerSQL =
        "INSERT INTO tbl_owners "
        "(f_name, l_name, phone, u_id_fk) "
        "VALUES (?, ?, ?, ?)";

    if (mysql_stmt_prepare(stmt, ownerSQL, strlen(ownerSQL)))
    {
        printf("Owner prepare failed: %s\n",
               mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        mysql_query(conn, "ROLLBACK");
        memset(hashedPassword, 0, sizeof(hashedPassword));
        return 0;
    }

    //Owner bindings

    MYSQL_BIND ownerBind[4];

    unsigned long firstLength = strlen(firstName);
    unsigned long lastLength = strlen(lastName);
    unsigned long phoneLength = strlen(phone);

    memset(ownerBind, 0, sizeof(ownerBind));

    ownerBind[0].buffer_type = MYSQL_TYPE_STRING;
    ownerBind[0].buffer = firstName;
    ownerBind[0].buffer_length = sizeof(firstName);
    ownerBind[0].length = &firstLength;

    ownerBind[1].buffer_type = MYSQL_TYPE_STRING;
    ownerBind[1].buffer = lastName;
    ownerBind[1].buffer_length = sizeof(lastName);
    ownerBind[1].length = &lastLength;

    ownerBind[2].buffer_type = MYSQL_TYPE_STRING;
    ownerBind[2].buffer = phone;
    ownerBind[2].buffer_length = sizeof(phone);
    ownerBind[2].length = &phoneLength;

    ownerBind[3].buffer_type = MYSQL_TYPE_LONG;
    ownerBind[3].buffer = &userID;

    if (mysql_stmt_bind_param(stmt, ownerBind) ||
        mysql_stmt_execute(stmt))
    {
        printf("Owner insertion failed: %s\n",
               mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        mysql_query(conn, "ROLLBACK");
        memset(hashedPassword, 0, sizeof(hashedPassword));
        return 0;
    }

    mysql_stmt_close(stmt);



    if (mysql_query(conn, "COMMIT"))
    {
        printf("Commit failed: %s\n", mysql_error(conn));

        mysql_query(conn, "ROLLBACK");
        memset(hashedPassword, 0, sizeof(hashedPassword));
        return 0;
    }

    memset(hashedPassword, 0, sizeof(hashedPassword));

    printf("\nPump owner registered successfully!\n");
    printf("User ID: %d\n", userID);

    return 1;
}
