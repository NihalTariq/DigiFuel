
#include <stdio.h>
#include <mysql.h>
#include "admin.h"

void view_admins(MYSQL *conn)
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    //Fetch all admins with user info
    const char *query =
        "SELECT u.u_id, u.username, u.email "
        "FROM tbl_user u "
        "WHERE u.role = 'admin'";

    if (mysql_query(conn, query))
    {
        printf("Error: %s\n", mysql_error(conn));
        return;
    }

    result = mysql_store_result(conn);

    if (result == NULL)
    {
        printf("Could not retrieve Admin: %s\n",
               mysql_error(conn));
        return;
    }
    //Count Rows
    if (mysql_num_rows(result) == 0)
    {
        printf("\nNo admins found.\n");
        mysql_free_result(result);
        return;
    }

    printf("\n========== ADMINS ==========\n");

    while ((row = mysql_fetch_row(result)))
    {
        printf("\nAdmin ID: %s\n", row[0]);
        printf("Username: %s\n", row[1]);
        printf("Email: %s\n", row[2]);
        printf("--------------------------------\n");
    }
    //Clears from memory
    mysql_free_result(result);
}



//View Admin By ID
void view_admin_by_id(MYSQL *conn)
{
    int adminID;
    char query[500];

    MYSQL_RES *result;
    MYSQL_ROW row;

    printf("\n===== SEARCH ADMIN =====\n");
    printf("Enter Admin ID: ");

    if (scanf("%d", &adminID) != 1 || adminID <= 0)
    {
        printf("Invalid Admin ID!\n");
        return;
    }

    //Stores the query in a string 
    sprintf(query,
        "SELECT u.u_id, u.username, u.email "
        "FROM tbl_user u "
        "WHERE u.u_id = %d",
        adminID);

    //Executes the query    
    if (mysql_query(conn, query))
    {
        printf("Search failed: %s\n", mysql_error(conn));
        return;
    }


    //Store The Admin Info
    result = mysql_store_result(conn);

    if (result == NULL)
    {
        printf("Could not retrieve results!\n");
        return;
    }

    row = mysql_fetch_row(result);

    //Print Admin
    if (row != NULL)
    {
        printf("\n===== ADMIN DETAILS =====\n");
        printf("Admin ID: %s\n", row[0]);
        printf("Username: %s\n", row[1]);
        printf("Email: %s\n", row[2]);
    }
    else
    {
        printf("\nNo admin found with ID %d.\n", adminID);
    }

    mysql_free_result(result);
}