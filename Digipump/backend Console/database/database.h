#ifndef DATABASE_H
#define DATABASE_H

#include<mysql.h>

MYSQL *connDB();

void closeconn(MYSQL *conn);

#endif