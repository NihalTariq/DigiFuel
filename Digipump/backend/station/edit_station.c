
#include <stdio.h>
#include <string.h>
#include <mysql.h>
#include "station.h"
#include "../owner/owner.h"

int edit_station(MYSQL *conn, int stationID)
{
    int ownerID;

    char name[61];
    char license[31];
    char adress[256];
    char city[21];
    char status[21];

    MYSQL_STMT *stmt;
    MYSQL_BIND bind[7];

    unsigned long nameLength;
    unsigned long licenseLength;
    unsigned long adressLength;
    unsigned long cityLength;
    unsigned long statusLength;

    printf("\n========== UPDATE FUEL STATION ==========\n");

    if (stationID <= 0)
    {
        printf("Invalid Station ID.\n");
        return 0;
    }

    //Enter Edited Info
    printf("Enter new station name: ");
    scanf(" %60[^\n]", name);

    printf("Enter new license: ");
    scanf(" %30s", license);

    printf("Enter new address: ");
    scanf(" %255[^\n]", adress);

    printf("Enter new city: ");
    scanf(" %20s", city);

    printf("Enter new status: ");
    scanf(" %20s", status);

    //Select New Owner
    printf("\nAvailable Pump Owners:\n");
    view_owners(conn);

    printf("\nEnter new Owner ID: ");
    if (scanf("%d", &ownerID) != 1 || ownerID <= 0)
    {
        printf("Invalid Owner ID.\n");
        return 0;
    }

    const char *query =
        "UPDATE tbl_stations "
        "SET name = ?, license = ?, adress = ?, "
        "city = ?, s_status = ?, o_id_fk = ? "
        "WHERE s_id = ?";

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

    nameLength = strlen(name);
    licenseLength = strlen(license);
    adressLength = strlen(adress);
    cityLength = strlen(city);
    statusLength = strlen(status);

    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = name;
    bind[0].length = &nameLength;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = license;
    bind[1].length = &licenseLength;

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = adress;
    bind[2].length = &adressLength;

    bind[3].buffer_type = MYSQL_TYPE_STRING;
    bind[3].buffer = city;
    bind[3].length = &cityLength;

    bind[4].buffer_type = MYSQL_TYPE_STRING;
    bind[4].buffer = status;
    bind[4].length = &statusLength;

    bind[5].buffer_type = MYSQL_TYPE_LONG;
    bind[5].buffer = &ownerID;

    bind[6].buffer_type = MYSQL_TYPE_LONG;
    bind[6].buffer = &stationID;

    if (mysql_stmt_bind_param(stmt, bind) ||
        mysql_stmt_execute(stmt))
    {
        if (mysql_stmt_errno(stmt) == 1062)
            printf("This station license is already registered.\n");
        else if (mysql_stmt_errno(stmt) == 1452)
            printf("Owner does not exist.\n");
        else
            printf("Update failed: %s\n", mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        return 0;
    }

    if (mysql_stmt_affected_rows(stmt) == 0)
    {
        printf("No station found, or no changes were made.\n");
    }
    else
    {
        printf("Fuel station updated successfully!\n");
    }

    mysql_stmt_close(stmt);

    return 1;
}