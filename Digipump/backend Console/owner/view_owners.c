
#include <stdio.h>
#include <mysql.h>
#include "owner.h"

void view_owners(MYSQL *conn)
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    //Fetch all owners with user info
    const char *query =
        "SELECT o.o_id, o.f_name, o.l_name, o.phone, "
        "u.username, u.email "
        "FROM tbl_owners o "
        "JOIN tbl_user u ON o.u_id_fk = u.u_id";

    if (mysql_query(conn, query))
    {
        printf("Error: %s\n", mysql_error(conn));
        return;
    }

    result = mysql_store_result(conn);

    if (result == NULL)
    {
        printf("Could not retrieve owners: %s\n",
               mysql_error(conn));
        return;
    }
    //Count Rows
    if (mysql_num_rows(result) == 0)
    {
        printf("\nNo pump owners found.\n");
        mysql_free_result(result);
        return;
    }

    printf("\n========== PUMP OWNERS ==========\n");

    while ((row = mysql_fetch_row(result)))
    {
        printf("\nOwner ID: %s\n", row[0]);
        printf("Name: %s %s\n", row[1], row[2]);
        printf("Phone: %s\n", row[3]);
        printf("Username: %s\n", row[4]);
        printf("Email: %s\n", row[5]);
        printf("--------------------------------\n");
    }
    //Clears from memory
    mysql_free_result(result);
}



//View Owner By ID
void view_owner_by_id(MYSQL *conn)
{
    int ownerID;
    char query[500];

    MYSQL_RES *result;
    MYSQL_ROW row;

    printf("\n===== SEARCH PUMP OWNER =====\n");
    printf("Enter Owner ID: ");

    if (scanf("%d", &ownerID) != 1 || ownerID <= 0)
    {
        printf("Invalid Owner ID!\n");
        return;
    }

    //stores the query in a string 
    sprintf(query,
        "SELECT o.o_id, o.f_name, o.l_name, o.phone, "
        "u.username, u.email "
        "FROM tbl_owners o "
        "JOIN tbl_user u ON o.u_id_fk = u.u_id "
        "WHERE o.o_id = %d",
        ownerID);

    //Executes the query    
    if (mysql_query(conn, query))
    {
        printf("Search failed: %s\n", mysql_error(conn));
        return;
    }


    //Store The Owner Info
    result = mysql_store_result(conn);

    if (result == NULL)
    {
        printf("Could not retrieve results!\n");
        return;
    }

    row = mysql_fetch_row(result);

    //print The Owner Info
    if (row != NULL)
    {
        printf("\n===== OWNER DETAILS =====\n");
        printf("Owner ID: %s\n", row[0]);
        printf("First Name: %s\n", row[1]);
        printf("Last Name: %s\n", row[2]);
        printf("Phone: %s\n", row[3]);
        printf("Username: %s\n", row[4]);
        printf("Email: %s\n", row[5]);
    }
    else
    {
        printf("\nNo owner found with ID %d.\n", ownerID);
    }

    mysql_free_result(result);
}