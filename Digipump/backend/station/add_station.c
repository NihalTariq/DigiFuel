
#include <stdio.h>
#include <string.h>
#include <mysql.h>
#include "station.h"
#include "../owner/owner.h"

int add_station(MYSQL *conn)
{
    int ownerID;

    char name[61];
    char license[31];
    char adress[256];
    char city[21];
    char status[21];

    MYSQL_STMT *stmt;
    MYSQL_BIND bind[6];

    unsigned long nameLength;
    unsigned long licenseLength;
    unsigned long adressLength;
    unsigned long cityLength;
    unsigned long statusLength;

    printf("\n========== ADD FUEL STATION ==========\n");

    //Display Available Owners
    view_owners(conn);

    //Enter Station Info
    printf("\nEnter Owner ID: ");

    if (scanf("%d", &ownerID) != 1 || ownerID <= 0)
    {
        printf("Invalid Owner ID!\n");
        return 0;
    }

    printf("Enter Station Name: ");
    scanf(" %60[^\n]", name);

    printf("Enter License Number: ");
    scanf(" %30s", license);

    printf("Enter Station Address: ");
    scanf(" %255[^\n]", adress);

    printf("Enter City: ");
    scanf(" %20s", city);

    printf("Enter Station Status (Active/Inactive): ");
    scanf(" %20s", status);

    //Check if Owner Exists
    const char *checkOwnerSQL =
        "SELECT o_id FROM tbl_owners WHERE o_id = ?";

    stmt = mysql_stmt_init(conn);

    if (stmt == NULL)
    {
        printf("Statement initialization failed.\n");
        return 0;
    }

    if (mysql_stmt_prepare(stmt, checkOwnerSQL, strlen(checkOwnerSQL)))
    {
        printf("Prepare failed: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return 0;
    }

    MYSQL_BIND ownerBind[1];
    memset(ownerBind, 0, sizeof(ownerBind));

    ownerBind[0].buffer_type = MYSQL_TYPE_LONG;
    ownerBind[0].buffer = &ownerID;

    if (mysql_stmt_bind_param(stmt, ownerBind) ||
        mysql_stmt_execute(stmt))
    {
        printf("Owner verification failed: %s\n",
               mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return 0;
    }

    if (mysql_stmt_store_result(stmt))
    {
        printf("Unable to retrieve owner details.\n");
        mysql_stmt_close(stmt);
        return 0;
    }

    if (mysql_stmt_num_rows(stmt) == 0)
    {
        printf("\nNo owner found with ID %d.\n", ownerID);
        mysql_stmt_close(stmt);
        return 0;
    }

    mysql_stmt_close(stmt);

    //Check if License Already Exists
    const char *checkLicenseSQL =
        "SELECT s_id FROM tbl_stations WHERE license = ?";

    stmt = mysql_stmt_init(conn);

    if (stmt == NULL)
    {
        printf("Statement initialization failed.\n");
        return 0;
    }

    if (mysql_stmt_prepare(stmt, checkLicenseSQL,
                           strlen(checkLicenseSQL)))
    {
        printf("Prepare failed: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return 0;
    }

    MYSQL_BIND licenseBind[1];
    memset(licenseBind, 0, sizeof(licenseBind));

    licenseLength = strlen(license);

    licenseBind[0].buffer_type = MYSQL_TYPE_STRING;
    licenseBind[0].buffer = license;
    licenseBind[0].buffer_length = sizeof(license);
    licenseBind[0].length = &licenseLength;

    if (mysql_stmt_bind_param(stmt, licenseBind) ||
        mysql_stmt_execute(stmt))
    {
        printf("License verification failed: %s\n",
               mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return 0;
    }

    if (mysql_stmt_store_result(stmt))
    {
        printf("Unable to retrieve license details.\n");
        mysql_stmt_close(stmt);
        return 0;
    }

    if (mysql_stmt_num_rows(stmt) > 0)
    {
        printf("\nThis license number is already registered!\n");
        mysql_stmt_close(stmt);
        return 0;
    }

    mysql_stmt_close(stmt);

    //Insert Station Info
    const char *query =
        "INSERT INTO tbl_stations "
        "(name, license, adress, city, s_status, o_id_fk) "
        "VALUES (?, ?, ?, ?, ?, ?)";

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
    bind[0].buffer_length = sizeof(name);
    bind[0].length = &nameLength;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = license;
    bind[1].buffer_length = sizeof(license);
    bind[1].length = &licenseLength;

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = adress;
    bind[2].buffer_length = sizeof(adress);
    bind[2].length = &adressLength;

    bind[3].buffer_type = MYSQL_TYPE_STRING;
    bind[3].buffer = city;
    bind[3].buffer_length = sizeof(city);
    bind[3].length = &cityLength;

    bind[4].buffer_type = MYSQL_TYPE_STRING;
    bind[4].buffer = status;
    bind[4].buffer_length = sizeof(status);
    bind[4].length = &statusLength;

    bind[5].buffer_type = MYSQL_TYPE_LONG;
    bind[5].buffer = &ownerID;

    if (mysql_stmt_bind_param(stmt, bind) ||
        mysql_stmt_execute(stmt))
    {
        if (mysql_stmt_errno(stmt) == 1062)
        {
            printf("This license number is already registered!\n");
        }
        else if (mysql_stmt_errno(stmt) == 1452)
        {
            printf("Selected owner does not exist!\n");
        }
        else
        {
            printf("Station insertion failed: %s\n",
                   mysql_stmt_error(stmt));
        }

        mysql_stmt_close(stmt);
        return 0;
    }

    printf("\nFuel station added successfully!\n");
    printf("Station ID: %lu\n", mysql_stmt_insert_id(stmt));

    mysql_stmt_close(stmt);

    return 1;
}
