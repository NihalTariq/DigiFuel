
#include <stdio.h>
#include <string.h>
#include <mysql.h>

#include "auth.h"
#include "../utils/hash_password.h"

int signup(MYSQL *conn)
{
    char username[61];
    char email[61];
    char password[101];
    char hashedPassword[150];

    char firstName[61];
    char lastName[61];
    char phone[21];

    MYSQL_STMT *stmt;
    MYSQL_BIND bind[4];

    unsigned long usernameLength;
    unsigned long emailLength;
    unsigned long passwordLength;
    unsigned long roleLength;

    const char *role = "Citizen";

    //Login Info | will be added into tbl_user
    printf("\n===== SIGN UP =====\n");

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

    // Generate password hash
    if (!hashPassword(password, hashedPassword,
                      sizeof(hashedPassword)))
    {
        printf("Password hashing failed!\n");
        return 0;
    }

    //Clears the temporary password buffer
    memset(password, 0, sizeof(password));

    //Transaction to add in both tbl_user and tbl_citizen tables
    if (mysql_query(conn, "START TRANSACTION"))
    {
        printf("Transaction failed: %s\n", mysql_error(conn));
        return 0;
    }

    //Insertion Into tbl_user
    stmt = mysql_stmt_init(conn);

    if (stmt == NULL)
    {
        mysql_query(conn, "ROLLBACK");
        return 0;
    }

    const char *userSQL =
        "INSERT INTO tbl_user "
        "(username, email, userpass, role) "
        "VALUES (?, ?, ?, ?)";

    if (mysql_stmt_prepare(stmt, userSQL, strlen(userSQL)))
    {
        printf("Prepare failed: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_query(conn, "ROLLBACK");
        return 0;
    }

    memset(bind, 0, sizeof(bind));

    //Gets Length of Inputs
    usernameLength = strlen(username);
    emailLength = strlen(email);
    passwordLength = strlen(hashedPassword);
    roleLength = strlen(role);

    //The '?' will be replaced by this values in thi array
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
        printf("User insertion failed: %s\n",
               mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        mysql_query(conn, "ROLLBACK");
        return 0;
    }

    //Newly generated User ID from tbl_user
    int userID = (int)mysql_insert_id(conn);

    mysql_stmt_close(stmt);

    //Prepare citizen profile insertion
    stmt = mysql_stmt_init(conn);

    if (stmt == NULL)
    {
        mysql_query(conn, "ROLLBACK");
        return 0;
    }

    const char *citizenSQL =
        "INSERT INTO tbl_citizen "
        "(f_name, l_name, phone, u_id_fk) "
        "VALUES (?, ?, ?, ?)";

    if (mysql_stmt_prepare(stmt, citizenSQL, strlen(citizenSQL)))
    {
        printf("Citizen prepare failed: %s\n",
               mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        mysql_query(conn, "ROLLBACK");
        return 0;
    }

    //Citizen Bindings 
    MYSQL_BIND citizenBind[4];
    unsigned long firstLength = strlen(firstName);
    unsigned long lastLength = strlen(lastName);
    unsigned long phoneLength = strlen(phone);

    memset(citizenBind, 0, sizeof(citizenBind));

    citizenBind[0].buffer_type = MYSQL_TYPE_STRING;
    citizenBind[0].buffer = firstName;
    citizenBind[0].length = &firstLength;

    citizenBind[1].buffer_type = MYSQL_TYPE_STRING;
    citizenBind[1].buffer = lastName;
    citizenBind[1].length = &lastLength;

    citizenBind[2].buffer_type = MYSQL_TYPE_STRING;
    citizenBind[2].buffer = phone;
    citizenBind[2].length = &phoneLength;

    citizenBind[3].buffer_type = MYSQL_TYPE_LONG;
    citizenBind[3].buffer = &userID;

    if (mysql_stmt_bind_param(stmt, citizenBind) ||
        mysql_stmt_execute(stmt))
    {
        printf("Citizen insertion failed: %s\n",
               mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        mysql_query(conn, "ROLLBACK");
        return 0;
    }

    mysql_stmt_close(stmt);

    //Commit both insertions
    if (mysql_query(conn, "COMMIT"))
    {
        printf("Commit failed: %s\n", mysql_error(conn));
        mysql_query(conn, "ROLLBACK");
        return 0;
    }

    printf("\nCitizen registered successfully!\n");
    printf("User ID: %d\n", userID);

    return 1;
}