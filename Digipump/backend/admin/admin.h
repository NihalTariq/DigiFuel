#ifndef ADMIN_H
#define ADMIN_H
#include <mysql.h>

//Add Function
int add_admin(MYSQL *conn);

//View Functions
void view_admins(MYSQL *conn);
void view_admin_by_id(MYSQL *conn);

//Edit Functions
int edit_admin(MYSQL *conn);





#endif 