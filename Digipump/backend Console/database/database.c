
#include <stdio.h>
#include "database.h"


//Connection WITH Database
MYSQL *connDB(void)
{
    MYSQL *conn;

    unsigned char ssl_enforce = 0;
    unsigned char ssl_verify = 0;

    conn = mysql_init(NULL);

    if (conn == NULL)
    {
        printf("Database initialization failed!\n");
        return NULL;
    }

    mysql_options(conn, MYSQL_OPT_SSL_ENFORCE, &ssl_enforce);
    mysql_options(conn, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, &ssl_verify);

    if (mysql_real_connect(
            conn,
            "127.0.0.1",
            "root",
            "",
            "digipump", //Name of The database
            3306,
            NULL,
            0) == NULL)
    {
        printf("Database connection failed: %s\n", mysql_error(conn));

        mysql_close(conn);
        return NULL;
    }

    printf("Database connected successfully!\n");

    return conn;
}

//Closes Database Connection
void closeconn(MYSQL *conn)
{
    if (conn != NULL)
    {
        mysql_close(conn);
        printf("\n Database connection closed.\n");
    }
}
