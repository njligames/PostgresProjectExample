//
// Created by James Folk on 4/14/25.
//

#include "MosaifyDatabase.h"
#include "IImageData.h"
#include <cstdlib>  // For std::getenv
#include <libpq-fe.h>
#include <vector>
#include <sstream>
#include <cstring>

namespace NJLIC {

#define HANDLE_ERROR(conn, operation, additionalInfo) \
    handleError(conn, operation, additionalInfo, __FILE__, __LINE__, __func__)

    std::string MosaifyDatabase::handleError(PGconn* conn, const std::string& operation, const std::string& additionalInfo,
                            const char* file, int line, const char* func) {
        std::stringstream ss;
        ss << "Error during operation: " << operation << "\n";
        if (!additionalInfo.empty()) {
            ss << "Additional Information: " << additionalInfo << "\n";
        }
        ss << "PostgreSQL Error: " << PQerrorMessage(conn) << "\n";
        ss << "Location: " << file << ":" << line << " in function " << func << "\n";
        return ss.str();
    }

    bool MosaifyDatabase::executeSQL(PGconn* conn, const std::string &sql, std::string &error_message) {
        bool ret = false;
        PGresult* res = PQexec(conn, sql.c_str());

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            error_message = HANDLE_ERROR(conn, "Execute SQL", sql);

            ret = false;
        } else {
            ret = true;
        }
        PQclear(res);
        return ret;
    }

    bool MosaifyDatabase::createProject(PGconn* conn, int user_id, const std::string& project_name, std::string &error_message) {
        const char* sql = "INSERT INTO projecttable (user_id, project_name) VALUES ($1, $2)";
        std::string user_id_str = std::to_string(user_id);
        const char* paramValues[2] = { user_id_str.c_str(), project_name.c_str() };

        PGresult* res = PQexecParams(conn, sql, 2, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            error_message = HANDLE_ERROR(conn, "Create Project", sql);

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool MosaifyDatabase::readProject(PGconn* conn, int project_id, std::string &error_message) {
        const char* sql = "SELECT user_id, project_name FROM projecttable WHERE id = $1";
        std::string project_id_str = std::to_string(project_id);
        const char* paramValues[1] = { project_id_str.c_str() };

        PGresult* res = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            error_message = HANDLE_ERROR(conn, "Read Project", sql);

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

    bool MosaifyDatabase::updateProject(PGconn* conn, int project_id, const std::string& new_project_name, std::string &error_message) {
        const char* sql = "UPDATE projecttable SET project_name = $1 WHERE id = $2";
        std::string project_id_str = std::to_string(project_id);
        const char* paramValues[2] = { new_project_name.c_str(), project_id_str.c_str() };

        PGresult* res = PQexecParams(conn, sql, 2, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            error_message = HANDLE_ERROR(conn, "Update Project", sql);

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool MosaifyDatabase::deleteProject(PGconn* conn, int project_id, std::string &error_message) {
        const char* sql = "DELETE FROM projecttable WHERE id = $1";
        std::string project_id_str = std::to_string(project_id);
        const char* paramValues[1] = { project_id_str.c_str() };

        PGresult* res = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            error_message = HANDLE_ERROR(conn, "Delete Project", sql);

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool MosaifyDatabase::readImages(PGconn* conn, int project_id, std::vector<std::unique_ptr<IImageData>>& images, const std::function<std::unique_ptr<IImageData>()>& createImageFunc, std::string &error_message) {
        // Prepare the SQL query
        const char* sql = "SELECT id, filename, rows, cols, comps, data FROM images WHERE project_id = $1";
        const char* paramValues[1];
        std::string project_id_str = std::to_string(project_id);
        paramValues[0] = project_id_str.c_str();

        // Execute the query
        PGresult* res = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            error_message = HANDLE_ERROR(conn, "Read Images", sql);
            PQclear(res);
            return false; // Return an empty vector on error
        }

        // Process the results
        int num_rows = PQntuples(res);
        for (int i = 0; i < num_rows; ++i) {

            auto img = createImageFunc();
            std::string filename = PQgetvalue(res, i, 1);
            int rows = std::stoi(PQgetvalue(res, i, 2));
            int cols = std::stoi(PQgetvalue(res, i, 3));
            int comps = std::stoi(PQgetvalue(res, i, 4));
            std::vector<unsigned char> data(PQgetlength(res, i, 5));
            memcpy(data.data(), PQgetvalue(res, i, 5), data.size());

            img->setFilename(filename);
            img->setRows(rows);
            img->setCols(cols);
            img->setComps(comps);
            img->setData(data);

            images.push_back(std::move(img));
        }

        PQclear(res);
        return true;
    }

    bool MosaifyDatabase::createUser(PGconn* conn, const std::string& email, const std::string& first_name, const std::string& last_name, std::string &error_message) {
        const char* sql = "INSERT INTO usertable (email, first_name, last_name) VALUES ($1, $2, $3)";
        const char* paramValues[3] = { email.c_str(), first_name.c_str(), last_name.c_str() };

        PGresult* res = PQexecParams(conn, sql, 3, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            error_message = HANDLE_ERROR(conn, "Create User", sql);

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool MosaifyDatabase::readUser(PGconn* conn, int user_id, std::string &error_message) {
        const char* sql = "SELECT email, first_name, last_name FROM usertable WHERE id = $1";
        std::string user_id_str = std::to_string(user_id);
        const char* paramValues[1] = { user_id_str.c_str() };

        PGresult* res = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            error_message = HANDLE_ERROR(conn, "Read User", sql);

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

    bool MosaifyDatabase::updateUser(PGconn* conn, int user_id, const std::string& new_email, const std::string& new_first_name, const std::string& new_last_name, std::string &error_message) {
        const char* sql = "UPDATE usertable SET email = $1, first_name = $2, last_name = $3 WHERE id = $4";
        std::string user_id_str = std::to_string(user_id);
        const char* paramValues[4] = { new_email.c_str(), new_first_name.c_str(), new_last_name.c_str(), user_id_str.c_str() };

        PGresult* res = PQexecParams(conn, sql, 4, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            error_message = HANDLE_ERROR(conn, "Update User", sql);

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool MosaifyDatabase::deleteUser(PGconn* conn, int user_id, std::string &error_message) {
        const char* sql = "DELETE FROM usertable WHERE id = $1";
        std::string user_id_str = std::to_string(user_id);
        const char* paramValues[1] = { user_id_str.c_str() };

        PGresult* res = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            error_message = HANDLE_ERROR(conn, "Delete User", sql);

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool MosaifyDatabase::readProjects(PGconn* conn, int user_id, std::vector<int>& project_ids, std::string &error_message) {

        // Prepare the SQL query
        const char* sql = "SELECT id FROM projecttable WHERE user_id = $1";
        const char* paramValues[1];
        std::string user_id_str = std::to_string(user_id);
        paramValues[0] = user_id_str.c_str();

        // Execute the query
        PGresult* res = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            error_message = HANDLE_ERROR(conn, "Read Projects", sql);
            PQclear(res);
            return false; // Return an empty vector on error
        }

        // Process the results
        int num_rows = PQntuples(res);
        for (int i = 0; i < num_rows; ++i) {
            int project_id = std::stoi(PQgetvalue(res, i, 0));
            project_ids.push_back(project_id);
        }

        PQclear(res);
        return true;
    }

    bool MosaifyDatabase::createImage(PGconn* conn, int project_id, const IImageData& img, std::string &error_message) {
        // Prepare the SQL statement
        const char* sql = "INSERT INTO images (project_id, filename, rows, cols, comps, data) VALUES ($1, $2, $3, $4, $5, $6)";

        // Convert data to a format suitable for PostgreSQL
        const char* paramValues[6];
        int paramLengths[6];
        int paramFormats[6] = {0, 0, 0, 0, 0, 1}; // Last parameter (data) is binary

        // Set parameter values
        paramValues[0] = std::to_string(project_id).c_str();
        paramValues[1] = img.getFilename().c_str();
        paramValues[2] = std::to_string(img.getRows()).c_str();
        paramValues[3] = std::to_string(img.getCols()).c_str();
        paramValues[4] = std::to_string(img.getComps()).c_str();
        paramValues[5] = reinterpret_cast<const char*>(img.getData().data());
        paramLengths[5] = img.getData().size();

        // Execute the SQL statement
        PGresult* res = PQexecParams(conn, sql, 6, nullptr, paramValues, paramLengths, paramFormats, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            error_message = HANDLE_ERROR(conn, "Create Image", sql);

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool MosaifyDatabase::createImages(PGconn* conn, int project_id, const std::vector<std::unique_ptr<IImageData>>& images, std::string& error_message) {
        // Begin a transaction block
        PGresult* res = PQexec(conn, "BEGIN");
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            error_message = HANDLE_ERROR(conn, "Create Images - BEGIN", "");
            PQclear(res);
            return false;
        }
        PQclear(res);

        // Prepare the SQL statement
        const char* sql = "INSERT INTO images (project_id, filename, rows, cols, comps, data) VALUES ($1, $2, $3, $4, $5, $6)";

        // Convert project_id to string once
        std::string project_id_str = std::to_string(project_id);

        for (const auto& image : images) {
            const char* paramValues[6];
            int paramLengths[6];
            int paramFormats[6] = {0, 0, 0, 0, 0, 1}; // Last parameter (data) is binary

            // Set parameter values
            std::string rows_str = std::to_string(image->getRows());
            std::string cols_str = std::to_string(image->getCols());
            std::string comps_str = std::to_string(image->getComps());

            paramValues[0] = project_id_str.c_str();
            paramValues[1] = image->getFilename().c_str();
            paramValues[2] = rows_str.c_str();
            paramValues[3] = cols_str.c_str();
            paramValues[4] = comps_str.c_str();
            paramValues[5] = reinterpret_cast<const char*>(image->getData().data());
            paramLengths[5] = image->getData().size();

            // Execute the SQL statement
            res = PQexecParams(conn, sql, 6, nullptr, paramValues, paramLengths, paramFormats, 0);

            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                error_message = HANDLE_ERROR(conn, "Create Images", sql);
                PQclear(res);
                // Rollback the transaction if any insert fails
                PQexec(conn, "ROLLBACK");
                return false;
            }
            PQclear(res);
        }

        // Commit the transaction
        res = PQexec(conn, "COMMIT");
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            error_message = HANDLE_ERROR(conn, "Create Images - COMMIT", "");
            PQclear(res);
            return false;
        }
        PQclear(res);

        return true;
    }

    bool MosaifyDatabase::readImage(PGconn* conn, int image_id, int &project_id, IImageData &img, std::string &error_message) {
        const char* sql = "SELECT project_id, filename, rows, cols, comps, data FROM images WHERE id = $1";
        const char* paramValues[1];
        paramValues[0] = std::to_string(image_id).c_str();

        PGresult* res = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            error_message = HANDLE_ERROR(conn, "Read Image", sql);

            PQclear(res);
            return false;
        }

        if (PQntuples(res) == 0) {
            PQclear(res);
            return false;
        }

        int p_id = std::stoi(PQgetvalue(res, 0, 0));
        std::string filename = PQgetvalue(res, 0, 1);
        int rows = std::stoi(PQgetvalue(res, 0, 2));
        int cols = std::stoi(PQgetvalue(res, 0, 3));
        int comps = std::stoi(PQgetvalue(res, 0, 4));
        std::vector<unsigned char> data(PQgetlength(res, 0, 5));
        memcpy(data.data(), PQgetvalue(res, 0, 5), data.size());

        project_id = p_id;
        img.setFilename(filename);
        img.setRows(rows);
        img.setCols(cols);
        img.setComps(comps);
        img.setData(data);

        PQclear(res);
        return true;
    }

    bool MosaifyDatabase::updateImage(PGconn* conn, int image_id, const std::string& new_filename, int new_rows, int new_cols, int new_comps, const std::vector<unsigned char>& new_data, std::string &error_message) {
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
            error_message = HANDLE_ERROR(conn, "Update Image", sql);

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool MosaifyDatabase::deleteImage(PGconn* conn, int image_id, std::string &error_message) {
        const char* sql = "DELETE FROM images WHERE id = $1";
        const char* paramValues[1];
        paramValues[0] = std::to_string(image_id).c_str();

        PGresult* res = PQexecParams(conn, sql, 1, nullptr, paramValues, nullptr, nullptr, 0);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            error_message = HANDLE_ERROR(conn, "Delete Image", sql);

            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }


    bool MosaifyDatabase::executeSQL(const MosaifyDatabase &db, const std::string &sql, std::string &error_message) {
       return MosaifyDatabase::executeSQL(db.m_conn, sql, error_message);
    }

    MosaifyDatabase::MosaifyDatabase() : m_conn(nullptr) {

    }
    MosaifyDatabase::~MosaifyDatabase() {
        disconnect();
    }

    bool MosaifyDatabase::connect(const std::string connectionString, std::string &error_message) {
        disconnect();

        m_conn = PQconnectdb(connectionString.c_str());

        if (PQstatus(m_conn) != CONNECTION_OK) {
            error_message = HANDLE_ERROR(m_conn, "Connecting", "");

            PQfinish(m_conn);
            return false;
        }

        return true;
    }

    void MosaifyDatabase::disconnect() {
        if(nullptr == m_conn)
            PQfinish(m_conn);
        m_conn = nullptr;
    }

    bool MosaifyDatabase::createTables(bool reset, std::string &error_message) {
        if(reset) {
            // SQL statements to drop tables if they exist
            const char* dropImagesTableSQL = "DROP TABLE IF EXISTS images CASCADE;";
            const char* dropProjectTableSQL = "DROP TABLE IF EXISTS projecttable CASCADE;";
            const char* dropUserTableSQL = "DROP TABLE IF EXISTS usertable CASCADE;";

            // Execute SQL statements to drop tables
            if(!NJLIC::MosaifyDatabase::executeSQL(m_conn, dropImagesTableSQL, error_message))return false;
            if(!NJLIC::MosaifyDatabase::executeSQL(m_conn, dropProjectTableSQL, error_message))return false;
            if(!NJLIC::MosaifyDatabase::executeSQL(m_conn, dropUserTableSQL, error_message))return false;
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

    bool MosaifyDatabase::reset(std::string &error_message) {
        return createTables(true, error_message);
    }

    bool MosaifyDatabase::createProject(int user_id, const std::string& project_name, std::string &error_message) {
        return MosaifyDatabase::createProject(m_conn, user_id, project_name, error_message);
    }

    bool MosaifyDatabase::readProject(int project_id, std::string &error_message) {
        return MosaifyDatabase::readProject(m_conn, project_id, error_message);
    }

    bool MosaifyDatabase::updateProject(int project_id, const std::string& new_project_name, std::string &error_message) {
        return MosaifyDatabase::updateProject(m_conn, project_id, new_project_name, error_message);
    }

    bool MosaifyDatabase::deleteProject(int project_id, std::string &error_message) {
        return MosaifyDatabase::deleteProject(m_conn, project_id, error_message);
    }

    bool MosaifyDatabase::readImages(int project_id, std::vector<std::unique_ptr<IImageData>>& images, const std::function<std::unique_ptr<IImageData>()>& createImageFunc, std::string &error_message) {
       return MosaifyDatabase::readImages(m_conn, project_id, images, createImageFunc, error_message);
    }

    bool MosaifyDatabase::createUser(const std::string& email, const std::string& first_name, const std::string& last_name, std::string &error_message) {
        return MosaifyDatabase::createUser(m_conn, email, first_name, last_name, error_message);
    }

    bool MosaifyDatabase::readUser(int user_id, std::string &error_message) {
        return MosaifyDatabase::readUser(m_conn, user_id, error_message);
    }

    bool MosaifyDatabase::updateUser(int user_id, const std::string& new_email, const std::string& new_first_name, const std::string& new_last_name, std::string &error_message) {
        return MosaifyDatabase::updateUser(m_conn, user_id, new_email, new_first_name, new_last_name, error_message);
    }

    bool MosaifyDatabase::deleteUser(int user_id, std::string &error_message) {
        return MosaifyDatabase::deleteUser(m_conn, user_id, error_message);
    }

    bool MosaifyDatabase::readProjects(int user_id, std::vector<int>& project_ids, std::string &error_message) {
        return MosaifyDatabase::readProjects(m_conn, user_id, project_ids, error_message);
    }

    bool MosaifyDatabase::createImage(int project_id, const IImageData& img, std::string &error_message) {
        return MosaifyDatabase::createImage(m_conn, project_id, img, error_message);
    }
    bool MosaifyDatabase::createImages(int project_id, const std::vector<std::unique_ptr<IImageData>>& images, std::string& error_message) {
        return MosaifyDatabase::createImages(m_conn, project_id, images, error_message);
    }

    bool MosaifyDatabase::readImage(int image_id, int &project_id, IImageData &img, std::string &error_message) {
        return MosaifyDatabase::readImage(m_conn, image_id, project_id, img, error_message);
    }

    bool MosaifyDatabase::updateImage(int image_id, const std::string& new_filename, int new_rows, int new_cols, int new_comps, const std::vector<unsigned char>& new_data, std::string &error_message) {
        return MosaifyDatabase::updateImage(m_conn, image_id, new_filename, new_rows, new_cols, new_comps, new_data, error_message);
    }

    bool MosaifyDatabase::deleteImage(int image_id, std::string &error_message) {
        return MosaifyDatabase::deleteImage(m_conn, image_id, error_message);
    }

} // NJLIC