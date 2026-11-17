
#include <stdio.h>
#include <string.h>
#include <mysql.h>
#include "owner.h"

int edit_owner(MYSQL *conn)
{
    int ownerID;

    char firstName[61];
    char lastName[61];
    char phone[21];

    MYSQL_STMT *stmt;
    MYSQL_BIND bind[4];

    unsigned long firstLength;
    unsigned long lastLength;
    unsigned long phoneLength;

    printf("\n========== UPDATE OWNER ==========\n");

    printf("Enter Owner ID: ");
    if (scanf("%d", &ownerID) != 1 || ownerID <= 0)
    {
        printf("Invalid Owner ID.\n");
        return 0;
    }


    //Enter Edited Info
    printf("Enter new first name: ");
    scanf(" %60s", firstName);

    printf("Enter new last name: ");
    scanf(" %60s", lastName);

    printf("Enter new phone: ");
    scanf(" %20s", phone);

    const char *query =
        "UPDATE tbl_owners "
        "SET f_name = ?, l_name = ?, phone = ? "
        "WHERE o_id = ?";

    stmt = mysql_stmt_init(conn);

    if (stmt == NULL)
    {
        printf("Statement initialization failed.\n");
        return 0;
    }

    if (mysql_stmt_prepare(stmt, query, strlen(query)))
    {
        printf("Prepare failed: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return 0;
    }

    firstLength = strlen(firstName);
    lastLength = strlen(lastName);
    phoneLength = strlen(phone);

    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = firstName;
    bind[0].length = &firstLength;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = lastName;
    bind[1].length = &lastLength;

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = phone;
    bind[2].length = &phoneLength;

    bind[3].buffer_type = MYSQL_TYPE_LONG;
    bind[3].buffer = &ownerID;

    if (mysql_stmt_bind_param(stmt, bind) ||
        mysql_stmt_execute(stmt))
    {
        printf("Update failed: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return 0;
    }

    if (mysql_stmt_affected_rows(stmt) == 0)
    {
        printf("No owner found, or no changes were made.\n");
    }
    else
    {
        printf("Owner updated successfully!\n");
    }

    mysql_stmt_close(stmt);

    return 1;
}