
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#include "mongoose.h"
#include "server.h"
#include "../database/database.h"
#include "../authentication/auth.h"

// Temporary session (single-user testing only)
static UserSession session = {0};


// Read a JSON value into a character array
static void getJSONValue(struct mg_str body, const char *path,
                         char *output, size_t outputSize)
{
    char *value = mg_json_get_str(body, path);

    if (value == NULL)
        return;

    snprintf(output, outputSize, "%s", value);
    free(value);
}


// Handle HTTP requests
static void eventHandler(struct mg_connection *c, int ev, void *ev_data)
{
    if (ev != MG_EV_HTTP_MSG)
        return;

    struct mg_http_message *hm = (struct mg_http_message *) ev_data;


    // =========================
    // HEALTH CHECK
    // =========================

    if (mg_match(hm->uri, mg_str("/api/health"), NULL))
    {
        mg_http_reply(c, 200,
            "Content-Type: application/json\r\n",
            "{\"success\":true,\"message\":\"Server running\"}");

        return;
    }


    // =========================
    // SIGNUP
    // =========================

    if (mg_match(hm->uri, mg_str("/api/signup"), NULL))
    {
        if (!mg_match(hm->method, mg_str("POST"), NULL))
        {
            mg_http_reply(c, 405,
                "Content-Type: application/json\r\n",
                "{\"success\":false,\"message\":\"POST required\"}");

            return;
        }

        SignupData data = {0};

        getJSONValue(hm->body, "$.username",
                     data.username, sizeof(data.username));

        getJSONValue(hm->body, "$.email",
                     data.email, sizeof(data.email));

        getJSONValue(hm->body, "$.password",
                     data.password, sizeof(data.password));

        getJSONValue(hm->body, "$.firstName",
                     data.firstName, sizeof(data.firstName));

        getJSONValue(hm->body, "$.lastName",
                     data.lastName, sizeof(data.lastName));

        getJSONValue(hm->body, "$.phone",
                     data.phone, sizeof(data.phone));


        MYSQL *conn = connDB();

        if (conn == NULL)
        {
            memset(data.password, 0, sizeof(data.password));

            mg_http_reply(c, 500,
                "Content-Type: application/json\r\n",
                "{\"success\":false,\"message\":\"Database connection failed\"}");

            return;
        }

        int newUserID = 0;
        char error[256] = {0};

        int result = signup(
            conn,
            &data,
            &newUserID,
            error,
            sizeof(error)
        );

        closeconn(conn);

        memset(data.password, 0, sizeof(data.password));

        if (result)
        {
            mg_http_reply(c, 201,
                "Content-Type: application/json\r\n",
                "{\"success\":true,\"message\":\"Signup successful\",\"userID\":%d}",
                newUserID);
        }
        else
        {
            mg_http_reply(c, 400,
                "Content-Type: application/json\r\n",
                "{\"success\":false,\"message\":\"%s\"}",
                error);
        }

        return;
    }


    // =========================
    // LOGIN
    // =========================

    if (mg_match(hm->uri, mg_str("/api/login"), NULL))
    {
        if (!mg_match(hm->method, mg_str("POST"), NULL))
        {
            mg_http_reply(c, 405,
                "Content-Type: application/json\r\n",
                "{\"success\":false,\"message\":\"POST required\"}");

            return;
        }

        LoginData data = {0};

        getJSONValue(hm->body, "$.usernameEmail",
                     data.usernameEmail, sizeof(data.usernameEmail));

        getJSONValue(hm->body, "$.password",
                     data.password, sizeof(data.password));


        MYSQL *conn = connDB();

        if (conn == NULL)
        {
            memset(data.password, 0, sizeof(data.password));

            mg_http_reply(c, 500,
                "Content-Type: application/json\r\n",
                "{\"success\":false,\"message\":\"Database connection failed\"}");

            return;
        }

        char error[256] = {0};

        int result = login(
            conn,
            &data,
            &session,
            error,
            sizeof(error)
        );

        closeconn(conn);

        memset(data.password, 0, sizeof(data.password));

        if (result)
        {
            mg_http_reply(c, 200,
                "Content-Type: application/json\r\n",
                "{\"success\":true,\"message\":\"Login successful\",\"userID\":%d,\"role\":\"%s\"}",
                session.userID,
                session.role);
        }
        else
        {
            mg_http_reply(c, 401,
                "Content-Type: application/json\r\n",
                "{\"success\":false,\"message\":\"%s\"}",
                error);
        }

        return;
    }


    // =========================
    // LOGOUT
    // =========================

    if (mg_match(hm->uri, mg_str("/api/logout"), NULL))
    {
        if (!mg_match(hm->method, mg_str("POST"), NULL))
        {
            mg_http_reply(c, 405,
                "Content-Type: application/json\r\n",
                "{\"success\":false,\"message\":\"POST required\"}");

            return;
        }

        logout(&session);

        mg_http_reply(c, 200,
            "Content-Type: application/json\r\n",
            "{\"success\":true,\"message\":\"Logout successful\"}");

        return;
    }


    // =========================
    // UNKNOWN ROUTE
    // =========================

    mg_http_reply(c, 404,
        "Content-Type: application/json\r\n",
        "{\"success\":false,\"message\":\"Route not found\"}");
}


// Start HTTP server
void startServer(void)
{
    struct mg_mgr manager;

    mg_mgr_init(&manager);

    struct mg_connection *listener = mg_http_listen(
        &manager,
        "http://0.0.0.0:8000",
        eventHandler,
        NULL
    );

    if (listener == NULL)
    {
        printf("Server failed to start!\n");

        mg_mgr_free(&manager);

        return;
    }

    printf("\n=================================\n");
    printf("         DIGI FUELS SERVER\n");
    printf("=================================\n");

    printf("Server: http://localhost:8000\n");
    printf("Health: GET /api/health\n");
    printf("Signup: POST /api/signup\n");
    printf("Login:  POST /api/login\n");
    printf("Logout: POST /api/logout\n");

    printf("\nPress Ctrl+C to stop the server.\n");

    while (1)
    {
        mg_mgr_poll(&manager, 1000);
    }

    mg_mgr_free(&manager);
}