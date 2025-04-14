//
// Created by James Folk on 4/14/25.
//

#include "Database.h"
#include <cstdlib>  // For std::getenv
#include <libpq-fe.h>
#include <vector>
#include <sstream>

namespace NJLIC {
    bool Database::executeSQL(PGconn* conn, const std::string &sql, std::string &error_message) {
        std::stringstream ss;
        bool ret = false;
        PGresult* res = PQexec(conn, sql.c_str());

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            ss << "SQL error: " << PQerrorMessage(conn);
            error_message = ss.str();

            ret = false;
        } else {
            ret = true;
        }
        PQclear(res);
        return ret;
    }

    bool Database::createProject(PGconn* conn, int user_id, const std::string& project_name, std::string &error_message) {
        std::stringstream ss;
        const char* sql = "INSERT INTO projecttable (user_id, project_name) VALUES ($1, $2)";
        std::string user_id_str = std::to_string(user_id);
        const char* paramValues[2] = { user_id_str.c_str(), project_name.c_str() };

        PGresult* res = PQexecParams(conn, sql, 2, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            ss << "Add project failed: " << PQerrorMessage(conn) << std::endl;
            error_message = ss.str();

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool Database::readProject(PGconn* conn, int project_id, std::string &error_message) {
        std::stringstream ss;
        const char* sql = "SELECT user_id, project_name FROM projecttable WHERE id = $1";
        std::string project_id_str = std::to_string(project_id);
        const char* paramValues[1] = { project_id_str.c_str() };

        PGresult* res = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            ss << "Read project failed: " << PQerrorMessage(conn) << std::endl;
            error_message = ss.str();

            PQclear(res);
            return false;
        }

        if (PQntuples(res) == 0) {
            PQclear(res);
            return false;
        }

        int user_id = std::stoi(PQgetvalue(res, 0, 0));
        std::string project_name = PQgetvalue(res, 0, 1);

        PQclear(res);
        return true;
    }

    bool Database::updateProject(PGconn* conn, int project_id, const std::string& new_project_name, std::string &error_message) {
        std::stringstream ss;
        const char* sql = "UPDATE projecttable SET project_name = $1 WHERE id = $2";
        std::string project_id_str = std::to_string(project_id);
        const char* paramValues[2] = { new_project_name.c_str(), project_id_str.c_str() };

        PGresult* res = PQexecParams(conn, sql, 2, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            ss << "Update project failed: " << PQerrorMessage(conn) << std::endl;
            error_message = ss.str();

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool Database::deleteProject(PGconn* conn, int project_id, std::string &error_message) {
        std::stringstream ss;
        const char* sql = "DELETE FROM projecttable WHERE id = $1";
        std::string project_id_str = std::to_string(project_id);
        const char* paramValues[1] = { project_id_str.c_str() };

        PGresult* res = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            ss << "Delete project failed: " << PQerrorMessage(conn) << std::endl;
            error_message = ss.str();

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool Database::createUser(PGconn* conn, const std::string& email, const std::string& first_name, const std::string& last_name, std::string &error_message) {
        std::stringstream ss;
        const char* sql = "INSERT INTO usertable (email, first_name, last_name) VALUES ($1, $2, $3)";
        const char* paramValues[3] = { email.c_str(), first_name.c_str(), last_name.c_str() };

        PGresult* res = PQexecParams(conn, sql, 3, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            ss << "Add user failed: " << PQerrorMessage(conn) << std::endl;
            error_message = ss.str();

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool Database::readUser(PGconn* conn, int user_id, std::string &error_message) {
        std::stringstream ss;
        const char* sql = "SELECT email, first_name, last_name FROM usertable WHERE id = $1";
        std::string user_id_str = std::to_string(user_id);
        const char* paramValues[1] = { user_id_str.c_str() };

        PGresult* res = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            ss << "Read user failed: " << PQerrorMessage(conn) << std::endl;
            error_message = ss.str();

            PQclear(res);
            return false;
        }

        if (PQntuples(res) == 0) {
            PQclear(res);
            return false;
        }

        std::string email = PQgetvalue(res, 0, 0);
        std::string first_name = PQgetvalue(res, 0, 1);
        std::string last_name = PQgetvalue(res, 0, 2);

        PQclear(res);
        return true;
    }

    bool Database::updateUser(PGconn* conn, int user_id, const std::string& new_email, const std::string& new_first_name, const std::string& new_last_name, std::string &error_message) {
        std::stringstream ss;
        const char* sql = "UPDATE usertable SET email = $1, first_name = $2, last_name = $3 WHERE id = $4";
        std::string user_id_str = std::to_string(user_id);
        const char* paramValues[4] = { new_email.c_str(), new_first_name.c_str(), new_last_name.c_str(), user_id_str.c_str() };

        PGresult* res = PQexecParams(conn, sql, 4, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            ss << "Update user failed: " << PQerrorMessage(conn) << std::endl;
            error_message = ss.str();

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool Database::deleteUser(PGconn* conn, int user_id, std::string &error_message) {
        std::stringstream ss;
        const char* sql = "DELETE FROM usertable WHERE id = $1";
        std::string user_id_str = std::to_string(user_id);
        const char* paramValues[1] = { user_id_str.c_str() };

        PGresult* res = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            ss << "Delete user failed: " << PQerrorMessage(conn) << std::endl;
            error_message = ss.str();

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool Database::createImage(PGconn* conn, int project_id, const std::string& filename, int rows, int cols, int comps, const std::vector<unsigned char>& data, std::string &error_message) {
        std::stringstream ss;
        // Prepare the SQL statement
        const char* sql = "INSERT INTO images (project_id, filename, rows, cols, comps, data) VALUES ($1, $2, $3, $4, $5, $6)";

        // Convert data to a format suitable for PostgreSQL
        const char* paramValues[6];
        int paramLengths[6];
        int paramFormats[6] = {0, 0, 0, 0, 0, 1}; // Last parameter (data) is binary

        // Set parameter values
        paramValues[0] = std::to_string(project_id).c_str();
        paramValues[1] = filename.c_str();
        paramValues[2] = std::to_string(rows).c_str();
        paramValues[3] = std::to_string(cols).c_str();
        paramValues[4] = std::to_string(comps).c_str();
        paramValues[5] = reinterpret_cast<const char*>(data.data());
        paramLengths[5] = data.size();

        // Execute the SQL statement
        PGresult* res = PQexecParams(conn, sql, 6, nullptr, paramValues, paramLengths, paramFormats, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            ss << "Insert image failed: " << PQerrorMessage(conn) << std::endl;
            error_message = ss.str();

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool Database::readImage(PGconn* conn, int image_id, std::string &error_message) {
        std::stringstream ss;
        const char* sql = "SELECT project_id, filename, rows, cols, comps, data FROM images WHERE id = $1";
        const char* paramValues[1];
        paramValues[0] = std::to_string(image_id).c_str();

        PGresult* res = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            ss << "Read image failed: " << PQerrorMessage(conn) << std::endl;
            error_message = ss.str();

            PQclear(res);
            return false;
        }

        if (PQntuples(res) == 0) {
            PQclear(res);
            return false;
        }

        int project_id = std::stoi(PQgetvalue(res, 0, 0));
        std::string filename = PQgetvalue(res, 0, 1);
        int rows = std::stoi(PQgetvalue(res, 0, 2));
        int cols = std::stoi(PQgetvalue(res, 0, 3));
        int comps = std::stoi(PQgetvalue(res, 0, 4));
        std::vector<unsigned char> data(PQgetlength(res, 0, 5));
        memcpy(data.data(), PQgetvalue(res, 0, 5), data.size());

        PQclear(res);
        return true;
    }

    bool Database::updateImage(PGconn* conn, int image_id, const std::string& new_filename, int new_rows, int new_cols, int new_comps, const std::vector<unsigned char>& new_data, std::string &error_message) {
        std::stringstream ss;
        const char* sql = "UPDATE images SET filename = $1, rows = $2, cols = $3, comps = $4, data = $5 WHERE id = $6";
        const char* paramValues[6];
        int paramLengths[6];
        int paramFormats[6] = {0, 0, 0, 0, 1, 0}; // Data is binary

        paramValues[0] = new_filename.c_str();
        paramValues[1] = std::to_string(new_rows).c_str();
        paramValues[2] = std::to_string(new_cols).c_str();
        paramValues[3] = std::to_string(new_comps).c_str();
        paramValues[4] = reinterpret_cast<const char*>(new_data.data());
        paramLengths[4] = new_data.size();
        paramValues[5] = std::to_string(image_id).c_str();

        PGresult* res = PQexecParams(conn, sql, 6, nullptr, paramValues, paramLengths, paramFormats, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            ss << "Update image failed: " << PQerrorMessage(conn) << std::endl;
            error_message = ss.str();

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool Database::deleteImage(PGconn* conn, int image_id, std::string &error_message) {
        std::stringstream ss;
        const char* sql = "DELETE FROM images WHERE id = $1";
        const char* paramValues[1];
        paramValues[0] = std::to_string(image_id).c_str();

        PGresult* res = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            ss << "Delete image failed: " << PQerrorMessage(conn) << std::endl;
            error_message = ss.str();

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool Database::executeSQL(const Database &db, const std::string &sql, std::string &error_message) {
       return Database::executeSQL(db.m_conn, sql, error_message);
    }

    Database::Database() : m_conn(nullptr) {

    }
    Database::~Database() {
        disconnect();
    }

    bool Database::connect(const std::string connectionString, std::string &error_message) {
        std::stringstream ss;
        disconnect();

        m_conn = PQconnectdb(connectionString.c_str());

        if (PQstatus(m_conn) != CONNECTION_OK) {
            ss << "Connection to database failed: " << PQerrorMessage(m_conn) << std::endl;
            error_message = ss.str();

            PQfinish(m_conn);
            return false;
        }

        return true;
    }

    void Database::disconnect() {
        if(nullptr == m_conn)
            PQfinish(m_conn);
        m_conn = nullptr;
    }

    bool Database::createTables(bool reset, std::string &error_message) {
        if(reset) {
            // SQL statements to drop tables if they exist
            const char* dropImagesTableSQL = "DROP TABLE IF EXISTS images CASCADE;";
            const char* dropProjectTableSQL = "DROP TABLE IF EXISTS projecttable CASCADE;";
            const char* dropUserTableSQL = "DROP TABLE IF EXISTS usertable CASCADE;";

            // Execute SQL statements to drop tables
            if(!NJLIC::Database::executeSQL(m_conn, dropImagesTableSQL, error_message))return false;
            if(!NJLIC::Database::executeSQL(m_conn, dropProjectTableSQL, error_message))return false;
            if(!NJLIC::Database::executeSQL(m_conn, dropUserTableSQL, error_message))return false;
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
        if(!executeSQL(m_conn, createUserTableSQL, error_message))return false;
        if(!executeSQL(m_conn, createProjectTableSQL, error_message))return false;
        if(!executeSQL(m_conn, createImagesTableSQL, error_message))return false;
        return true;
    }

    bool Database::reset(std::string &error_message) {
        return createTables(true, error_message);
    }

    bool Database::createProject(int user_id, const std::string& project_name, std::string &error_message) {
        return Database::createProject(m_conn, user_id, project_name, error_message);
    }

    bool Database::readProject(int project_id, std::string &error_message) {
        return Database::readProject(m_conn, project_id, error_message);
    }

    bool Database::updateProject(int project_id, const std::string& new_project_name, std::string &error_message) {
        return Database::updateProject(m_conn, project_id, new_project_name, error_message);
    }

    bool Database::deleteProject(int project_id, std::string &error_message) {
        return Database::deleteProject(m_conn, project_id, error_message);
    }

    bool Database::createUser(const std::string& email, const std::string& first_name, const std::string& last_name, std::string &error_message) {
        return Database::createUser(m_conn, email, first_name, last_name, error_message);
    }

    bool Database::readUser(int user_id, std::string &error_message) {
        return Database::readUser(m_conn, user_id, error_message);
    }

    bool Database::updateUser(int user_id, const std::string& new_email, const std::string& new_first_name, const std::string& new_last_name, std::string &error_message) {
        return Database::updateUser(m_conn, user_id, new_email, new_first_name, new_last_name, error_message);
    }

    bool Database::deleteUser(int user_id, std::string &error_message) {
        return Database::deleteUser(m_conn, user_id, error_message);
    }

    bool Database::createImage(int project_id, const std::string& filename, int rows, int cols, int comps, const std::vector<unsigned char>& data, std::string &error_message) {
        return Database::createImage(m_conn, project_id, filename, rows, cols, comps, data, error_message);
    }

    bool Database::readImage(int image_id, std::string &error_message) {
        return Database::readImage(m_conn, image_id, error_message);
    }

    bool Database::updateImage(int image_id, const std::string& new_filename, int new_rows, int new_cols, int new_comps, const std::vector<unsigned char>& new_data, std::string &error_message) {
        return Database::updateImage(m_conn, image_id, new_filename, new_rows, new_cols, new_comps, new_data, error_message);
    }

    bool Database::deleteImage(int image_id, std::string &error_message) {
        return Database::deleteImage(m_conn, image_id, error_message);
    }

} // NJLIC