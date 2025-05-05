//
// Created by James Folk on 4/14/25.
//

#include <string>
//#include <libpq-fe.h>
#include <vector>
#include <functional>

#ifndef MYPROJECT_DATABASE_H
#define MYPROJECT_DATABASE_H

namespace NJLIC {

    class IImageData;

    class MosaifyDatabase {
    private:

    public:
        bool executeSQL(const std::string &sql, std::string &error_message);

        MosaifyDatabase();
        ~MosaifyDatabase();

        bool connect(const std::string connectionString, std::string &error_message);
        void disconnect();

        bool createTables(bool reset, std::string &error_message);
        bool reset(std::string &error_message);

        bool createMosaicImage(int project_id, const std::unique_ptr<IImageData> &img, int &image_id, std::string &error_message);
        bool readMosaicImage(int project_id, std::unique_ptr<IImageData> &img, std::string& error_message);
        bool updateMosaicImage(int project_id, const std::unique_ptr<IImageData> &img, std::string& error_message);
        bool deleteMosaicImage(int project_id, std::string& error_message);
        bool doesMosaicImageExist(int project_id, std::string& error_message);

        bool createMosaicMap(int project_id, const std::string& mosaic_map, std::string &error_message);
        bool readMosaicMap(int project_id, std::string& mosaic_map, std::string &error_message);
        bool updateMosaicMap(int project_id, const std::string& mosaic_map, std::string &error_message);
        bool deleteMosaicMap(int project_id, std::string &error_message);
        bool doesMosaicMapExist(int project_id, std::string& error_message);

        bool createProject(int user_id, const std::string& project_name, int &project_id, std::string &error_message);
        bool readProject(int project_id, int &user_id, std::string &project_name, std::string &error_message);
        bool updateProject(int project_id, const std::string& new_project_name, std::string &error_message);
        bool deleteProject(int project_id, std::string &error_message);
        bool readImages(int project_id, std::vector<std::unique_ptr<IImageData>>& images, const std::function<std::unique_ptr<IImageData>()>& createImageFunc, std::vector<int> &image_ids, std::string &error_message);

        bool createUser(const std::string& email, const std::string& first_name, const std::string& last_name, int &user_id, std::string &error_message);
        bool readUser(int user_id, std::string &error_message);
        bool updateUser(int user_id, const std::string& new_email, const std::string& new_first_name, const std::string& new_last_name, std::string &error_message);
        bool deleteUser(int user_id, std::string &error_message);
        bool readProjects(int user_id, std::vector<int>& project_ids, std::string &error_message);

        bool createImage(int project_id, std::unique_ptr<IImageData> img, int &image_id, std::string &error_message);
        bool createImages(int project_id, const std::vector<std::unique_ptr<IImageData>>& images, std::vector<int> &image_ids, std::string& error_message);
        bool readImage(int image_id, int project_id, std::unique_ptr<IImageData> &img, std::string &error_message);
        bool updateImage(int image_id, const std::string& new_filename, int new_rows, int new_cols, int new_comps, const std::vector<unsigned char>& new_data, std::string &error_message);
        bool deleteImage(int image_id, std::string &error_message);
    };

} // NJLIC

#endif //MYPROJECT_DATABASE_H
