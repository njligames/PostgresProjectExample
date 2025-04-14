#include <iostream>
#include <cstdlib>  // For std::getenv
#include <libpq-fe.h>

void executeSQL(PGconn* conn, const char* sql) {
    PGresult* res = PQexec(conn, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
    } else {
        std::cout << "Operation executed successfully" << std::endl;
    }
    PQclear(res);
}

int main(int argc, char* argv[]) {
    // Retrieve the connection string from the environment variable
    const char* conninfo = std::getenv("DB_CONN_STRING");
    if (!conninfo) {
        std::cerr << "Environment variable DB_CONN_STRING is not set." << std::endl;
        return 1;
    }

    PGconn* conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return 1;
    }

    // Check for the --reset command-line argument
    bool resetTables = true;
    if (argc > 1 && std::string(argv[1]) == "--reset") {
        resetTables = true;
    }

    if (resetTables) {
        // SQL statements to drop tables if they exist
        const char* dropImagesTableSQL = "DROP TABLE IF EXISTS images CASCADE;";
        const char* dropProjectTableSQL = "DROP TABLE IF EXISTS projecttable CASCADE;";
        const char* dropUserTableSQL = "DROP TABLE IF EXISTS usertable CASCADE;";

        // Execute SQL statements to drop tables
        executeSQL(conn, dropImagesTableSQL);
        executeSQL(conn, dropProjectTableSQL);
        executeSQL(conn, dropUserTableSQL);
    }

    // SQL statement to create the user table
    const char* createUserTableSQL = R"(
        CREATE TABLE IF NOT EXISTS usertable (
            id SERIAL PRIMARY KEY,
            email VARCHAR(255) NOT NULL,
            first_name VARCHAR(255) NOT NULL,
            last_name VARCHAR(255) NOT NULL
        );
    )";

    // SQL statement to create the project table
    const char* createProjectTableSQL = R"(
        CREATE TABLE IF NOT EXISTS projecttable (
            id SERIAL PRIMARY KEY,
            user_id INTEGER NOT NULL,
            project_name VARCHAR(255) NOT NULL,
            FOREIGN KEY (user_id) REFERENCES usertable(id) ON DELETE CASCADE
        );
    )";

    // SQL statement to create the images table
    const char* createImagesTableSQL = R"(
        CREATE TABLE IF NOT EXISTS images (
            id SERIAL PRIMARY KEY,
            project_id INTEGER NOT NULL,
            filename VARCHAR(255) NOT NULL,
            rows INTEGER NOT NULL,
            cols INTEGER NOT NULL,
            comps INTEGER NOT NULL,
            data BYTEA NOT NULL,
            FOREIGN KEY (project_id) REFERENCES projecttable(id) ON DELETE CASCADE
        );
    )";

    // Execute SQL statements to create tables
    executeSQL(conn, createUserTableSQL);
    executeSQL(conn, createProjectTableSQL);
    executeSQL(conn, createImagesTableSQL);

    // Close the database connection
    PQfinish(conn);

    return 0;
}
//
//
//
//
//#include <iostream>
//#include <libpq-fe.h>
//
//void executeSQL(PGconn* conn, const char* sql) {
//    PGresult* res = PQexec(conn, sql);
//    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
//        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
//    } else {
//        std::cout << "Operation executed successfully" << std::endl;
//    }
//    PQclear(res);
//}
//
//int main(int argc, char* argv[]) {
//    auto connection_string = "postgresql://admin:O2Fx22Dc4BqY1LFwUB1nw2dICJjyOTNM@dpg-cvt9d949c44c738iktf0-a.ohio-postgres.render.com/dbmosaify";
//    PGconn* conn = PQconnectdb(connection_string);
//
//    if (PQstatus(conn) != CONNECTION_OK) {
//        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
//        PQfinish(conn);
//        return 1;
//    }
//
//    // Check for the --reset command-line argument
//    bool resetTables = true;
//    if (argc > 1 && std::string(argv[1]) == "--reset") {
//        resetTables = true;
//    }
//
//    if (resetTables) {
//        // SQL statements to drop tables if they exist
//        const char* dropImagesTableSQL = "DROP TABLE IF EXISTS images CASCADE;";
//        const char* dropProjectTableSQL = "DROP TABLE IF EXISTS projecttable CASCADE;";
//        const char* dropUserTableSQL = "DROP TABLE IF EXISTS usertable CASCADE;";
//
//        // Execute SQL statements to drop tables
//        executeSQL(conn, dropImagesTableSQL);
//        executeSQL(conn, dropProjectTableSQL);
//        executeSQL(conn, dropUserTableSQL);
//    }
//
//    // SQL statement to create the user table
//    const char* createUserTableSQL = R"(
//        CREATE TABLE IF NOT EXISTS usertable (
//            id SERIAL PRIMARY KEY,
//            email VARCHAR(255) NOT NULL,
//            first_name VARCHAR(255) NOT NULL,
//            last_name VARCHAR(255) NOT NULL
//        );
//    )";
//
//    // SQL statement to create the project table
//    const char* createProjectTableSQL = R"(
//        CREATE TABLE IF NOT EXISTS projecttable (
//            id SERIAL PRIMARY KEY,
//            user_id INTEGER NOT NULL,
//            project_name VARCHAR(255) NOT NULL,
//            FOREIGN KEY (user_id) REFERENCES usertable(id) ON DELETE CASCADE
//        );
//    )";
//
//    // SQL statement to create the images table
//    const char* createImagesTableSQL = R"(
//        CREATE TABLE IF NOT EXISTS images (
//            id SERIAL PRIMARY KEY,
//            project_id INTEGER NOT NULL,
//            filename VARCHAR(255) NOT NULL,
//            rows INTEGER NOT NULL,
//            cols INTEGER NOT NULL,
//            comps INTEGER NOT NULL,
//            data BYTEA NOT NULL,
//            FOREIGN KEY (project_id) REFERENCES projecttable(id) ON DELETE CASCADE
//        );
//    )";
//
//    // Execute SQL statements to create tables
//    executeSQL(conn, createUserTableSQL);
//    executeSQL(conn, createProjectTableSQL);
//    executeSQL(conn, createImagesTableSQL);
//
//    // Close the database connection
//    PQfinish(conn);
//
//    return 0;
//}
