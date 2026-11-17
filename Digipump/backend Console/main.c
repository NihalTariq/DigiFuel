
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <mysql.h>

#include "database/database.h"
#include "authentication/auth.h"
#include "admin/admin.h"
#include "owner/owner.h"
#include "station/station.h"


//Compare Roles
int roleEquals(const char *role, const char *expected)
{
    while (*role && *expected)
    {
        if (tolower((unsigned char)*role) !=
            tolower((unsigned char)*expected))
        {
            return 0;
        }

        role++;
        expected++;
    }

    return *role == '\0' && *expected == '\0';
}


//Get Menu Choice
int getChoice()
{
    int choice;

    if (scanf("%d", &choice) != 1)
    {
        int ch;

        while ((ch = getchar()) != '\n' && ch != EOF);

        return -1;
    }

    return choice;
}


//Get Valid ID
int getID()
{
    int ID;

    printf("Enter ID: ");
    ID = getChoice();

    if (ID <= 0)
    {
        printf("Invalid ID!\n");
        return 0;
    }

    return ID;
}


//ADMIN DASHBOARD
void adminDashboard(MYSQL *conn)
{
    int choice;
    int stationID;

    do
    {
        printf("\n========== ADMIN DASHBOARD ==========\n");

        printf("\n--- ADMIN MANAGEMENT ---\n");
        printf("1. Add Admin\n");
        printf("2. View All Admins\n");

        printf("\n--- PUMP OWNER MANAGEMENT ---\n");
        printf("3. Add Pump Owner\n");
        printf("4. View All Pump Owners\n");
        printf("5. View Pump Owner By ID\n");
        printf("6. Edit Pump Owner\n");

        printf("\n--- FUEL STATION MANAGEMENT ---\n");
        printf("7. Add Fuel Station\n");
        printf("8. View All Fuel Stations\n");
        printf("9. Edit Fuel Station\n");

        printf("\n0. Logout\n");

        printf("\nEnter your choice: ");
        choice = getChoice();

        switch (choice)
        {
            case 1:
                add_admin(conn);
                break;

            case 2:
                view_admins(conn);
                break;

            case 3:
                add_owner(conn);
                break;

            case 4:
                view_owners(conn);
                break;

            case 5:
                view_owner_by_id(conn);
                break;

            case 6:
                edit_owner(conn);
                break;

            case 7:
                add_station(conn);
                break;

            case 8:
                view_stations(conn);
                break;

            case 9:
                stationID = getID();

                if (stationID > 0)
                {
                    edit_station(conn, stationID);
                }

                break;

            case 0:
                printf("\nLogging out...\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 0);
}


//PUMP OWNER DASHBOARD
void ownerDashboard(MYSQL *conn, UserSession *session)
{
    int choice;

    do
    {
        printf("\n========== PUMP OWNER DASHBOARD ==========\n");

        printf("1. View My Fuel Stations\n");

        printf("0. Logout\n");

        printf("\nEnter your choice: ");
        choice = getChoice();

        switch (choice)
        {
            case 1:
                view_owner_stations(conn, session->userID);
                break;

            case 0:
                printf("\nLogging out...\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 0);
}


//CITIZEN DASHBOARD
void citizenDashboard(UserSession *session)
{
    int choice;

    do
    {
        printf("\n========== CITIZEN DASHBOARD ==========\n");

        printf("Citizen User ID: %d\n", session->userID);

        printf("\nCitizen functionalities will be added later.\n");

        printf("\n0. Logout\n");
        printf("Enter your choice: ");

        choice = getChoice();

        if (choice != 0)
        {
            printf("Invalid choice!\n");
        }

    } while (choice != 0);
}


//EMPLOYEE DASHBOARD
void employeeDashboard(UserSession *session)
{
    int choice;

    do
    {
        printf("\n========== EMPLOYEE DASHBOARD ==========\n");

        printf("Employee User ID: %d\n", session->userID);

        printf("\nEmployee functionalities will be added later.\n");

        printf("\n0. Logout\n");
        printf("Enter your choice: ");

        choice = getChoice();

        if (choice != 0)
        {
            printf("Invalid choice!\n");
        }

    } while (choice != 0);
}


//MAIN FUNCTION
int main()
{
    MYSQL *conn;
    UserSession session;
    int choice;

    conn = connDB();

    if (conn == NULL)
    {
        printf("Database connection failed!\n");
        return 1;
    }

    session.userID = 0;
    session.role[0] = '\0';
    session.isLoggedIn = 0;

    do
    {
        printf("\n========== DIGI FUELS ==========\n");

        printf("1. Citizen Sign Up\n");
        printf("2. Login\n");
        printf("0. Exit\n");

        printf("\nEnter your choice: ");
        choice = getChoice();

        switch (choice)
        {
            case 1:
                signup(conn);
                break;

            case 2:

                if (login(conn, &session))
                {
                    printf("\nLogin successful!\n");

                    if (roleEquals(session.role, "admin"))
                    {
                        adminDashboard(conn);
                    }
                    else if (roleEquals(session.role, "owner"))
                    {
                        ownerDashboard(conn, &session);
                    }
                    else if (roleEquals(session.role, "citizen"))
                    {
                        citizenDashboard(&session);
                    }
                    else if (roleEquals(session.role, "employee"))
                    {
                        employeeDashboard(&session);
                    }
                    else
                    {
                        printf("Unknown user role!\n");
                    }

                    if (session.isLoggedIn)
                    {
                        logout(&session);
                    }
                }

                break;

            case 0:
                printf("\nExiting Digi Fuels...\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 0);

    closeconn(conn);

    return 0;
}
