
#include <stdio.h>
#include <mysql.h>
#include "station.h"

void view_stations(MYSQL *conn)
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    //Fetch all station with user info
    const char *query =
        "SELECT s.s_id, s.name, s.license, s.adress, s.city, s.s_status,o.f_name, o.l_name "
        "FROM tbl_stations s "
        "JOIN tbl_owners o ON s.o_id_fk = o.o_id";

    if (mysql_query(conn, query))
    {
        printf("Error: %s\n", mysql_error(conn));
        return;
    }

    result = mysql_store_result(conn);

    if (result == NULL)
    {
        printf("Could not retrieve stations: %s\n",
               mysql_error(conn));
        return;
    }
    //Count Rows
    if (mysql_num_rows(result) == 0)
    {
        printf("\nNo Fuel stations found.\n");
        mysql_free_result(result);
        return;
    }

    printf("\n========== Fuel STATIONS ==========\n");

    while ((row = mysql_fetch_row(result)))
    {
        printf("\n Fuel Station ID: %s\n", row[0]);
        printf("Name: %s %s\n", row[1]);
        printf("License: %s\n", row[2]);
        printf("Address: %s\n", row[3]);
        printf("City: %s\n", row[4]);
        printf("Status: %s\n", row[5]);
        printf("Owner: %s %s\n", row[6], row[7]);
        printf("--------------------------------\n");
    }
    //Clears from memory
    mysql_free_result(result);
}


//View  Station By Owner ID
void view_owner_stations(MYSQL *conn, int userID)
{
    char query[500];

    MYSQL_RES *result;
    MYSQL_ROW row;

    //
    snprintf(query, sizeof(query),
        "SELECT s.s_id, s.name, s.license, s.adress, "
        "s.city, s.s_status, o.f_name, o.l_name "
        "FROM tbl_stations s "
        "JOIN tbl_owners o ON s.o_id_fk = o.o_id "
        "WHERE o.u_id_fk = %d",
        userID);

    if (mysql_query(conn, query))
    {
        printf("Error: %s\n", mysql_error(conn));
        return;
    }

    result = mysql_store_result(conn);

    if (result == NULL)
    {
        printf("Could not retrieve stations: %s\n",
               mysql_error(conn));
        return;
    }

    //Count Rows
    if (mysql_num_rows(result) == 0)
    {
        printf("\nNo Fuel stations found.\n");
        mysql_free_result(result);
        return;
    }

    printf("\n========== YOUR FUEL STATIONS ==========\n");

    while ((row = mysql_fetch_row(result)))
    {
        printf("\nFuel Station ID: %s\n", row[0]);
        printf("Name: %s\n", row[1]);
        printf("License: %s\n", row[2]);
        printf("Address: %s\n", row[3]);
        printf("City: %s\n", row[4]);
        printf("Status: %s\n", row[5]);
        printf("Owner: %s %s\n", row[6], row[7]);
        printf("--------------------------------\n");
    }

    //Clears from memory
    mysql_free_result(result);
}