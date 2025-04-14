//
// Created by James Folk on 4/14/25.
//

#include <string>
#include <libpq-fe.h>
#include <vector>

#ifndef MYPROJECT_DATABASE_H
#define MYPROJECT_DATABASE_H

namespace NJLIC {

    class MosaifyDatabase {
    private:
        PGconn* m_conn;

        static bool executeSQL(PGconn* conn, const std::string &sql, std::string &error_message);

        static bool createProject(PGconn* conn, int user_id, const std::string& project_name, std::string &error_message);
        static bool readProject(PGconn* conn, int project_id, std::string &error_message);
        static bool updateProject(PGconn* conn, int project_id, const std::string& new_project_name, std::string &error_message);
        static bool deleteProject(PGconn* conn, int project_id, std::string &error_message);

        static bool createUser(PGconn* conn, const std::string& email, const std::string& first_name, const std::string& last_name, std::string &error_message);
        static bool readUser(PGconn* conn, int user_id, std::string &error_message);
        static bool updateUser(PGconn* conn, int user_id, const std::string& new_email, const std::string& new_first_name, const std::string& new_last_name, std::string &error_message);
        static bool deleteUser(PGconn* conn, int user_id, std::string &error_message);

        static bool createImage(PGconn* conn, int project_id, const std::string& filename, int rows, int cols, int comps, const std::vector<unsigned char>& data, std::string &error_message);
        static bool readImage(PGconn* conn, int image_id, std::string &error_message);
        static bool updateImage(PGconn* conn, int image_id, const std::string& new_filename, int new_rows, int new_cols, int new_comps, const std::vector<unsigned char>& new_data, std::string &error_message);
        static bool deleteImage(PGconn* conn, int image_id, std::string &error_message);

    public:
        static bool executeSQL(const MosaifyDatabase &db, const std::string &sql, std::string &error_message);

        MosaifyDatabase();
        ~MosaifyDatabase();

        bool connect(const std::string connectionString, std::string &error_message);
        void disconnect();

        bool createTables(bool reset, std::string &error_message);
        bool reset(std::string &error_message);

        bool createProject(int user_id, const std::string& project_name, std::string &error_message);
        bool readProject(int project_id, std::string &error_message);
        bool updateProject(int project_id, const std::string& new_project_name, std::string &error_message);
        bool deleteProject(int project_id, std::string &error_message);

        bool createUser(const std::string& email, const std::string& first_name, const std::string& last_name, std::string &error_message);
        bool readUser(int user_id, std::string &error_message);
        bool updateUser(int user_id, const std::string& new_email, const std::string& new_first_name, const std::string& new_last_name, std::string &error_message);
        bool deleteUser(int user_id, std::string &error_message);

        bool createImage(int project_id, const std::string& filename, int rows, int cols, int comps, const std::vector<unsigned char>& data, std::string &error_message);
        bool readImage(int image_id, std::string &error_message);
        bool updateImage(int image_id, const std::string& new_filename, int new_rows, int new_cols, int new_comps, const std::vector<unsigned char>& new_data, std::string &error_message);
        bool deleteImage(int image_id, std::string &error_message);
    };

} // NJLIC

#endif //MYPROJECT_DATABASE_H
