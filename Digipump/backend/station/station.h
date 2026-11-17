
#ifndef STATION_H
#define STATION_H

#include <mysql.h>

int add_station(MYSQL *conn);
void view_stations(MYSQL *conn);
void view_owner_stations(MYSQL *conn, int userID);
int edit_station(MYSQL *conn, int stationID);

#endif