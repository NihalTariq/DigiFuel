#ifndef OWNERS_H
#define OWNERS_H
#include <mysql.h>

//Add Function
int add_owner(MYSQL *conn);

//View Functions
void view_owners(MYSQL *conn);
void view_owner_by_id(MYSQL *conn);

//Edit Functions
int edit_owner(MYSQL *conn);





#endif 