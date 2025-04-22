#include <gtest/gtest.h>
#include "MosaifyDatabase/MosaifyDatabase.h"  // Include your database header
#include "MosaifyDatabase/IImageData.h"

#include <string>
#include <memory>
#include <vector>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace NJLIC;

class ImageData : public IImageData {
private:
    std::string filename;
    int rows;
    int cols;
    int comps; // Number of components (e.g., color channels)
    std::vector<unsigned char> data;

public:

//    images.push_back(std::make_unique<ImageData>("image2.png", 200, 200, 3, std::vector<unsigned char>{5, 6, 7, 8, 9}));
    ImageData() : rows(0), cols(0), comps(0) {}
    ImageData(const std::string &fname, int r, int c, int cps, const std::vector<unsigned char>&d) : filename(fname), rows(r), cols(c), comps(cps), data(d) {}

    // Getters
    const std::string &getFilename() const override {
        return filename;
    }

    int getRows() const override {
        return rows;
    }

    int getCols() const override {
        return cols;
    }

    int getComps() const override {
        return comps;
    }

    const std::vector<unsigned char>& getData() const override {
        return data;
    }

    // Setters
    void setFilename(const std::string& fname) override {
        filename = fname;
    }

    void setRows(int r) override {
        rows = r;
    }

    void setCols(int c) override {
        cols = c;
    }

    void setComps(int c) override {
        comps = c;
    }

    void setData(const std::vector<unsigned char>& d) override {
        data = d;
    }

//    // Load image data from a file
//    void loadFromFile(const std::string& filePath) {
//        std::ifstream file(filePath, std::ios::binary);
//        if (!file) {
//            throw std::runtime_error("Failed to open file: " + filePath);
//        }
//        data.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//        filename = filePath;
//        // Additional logic to set rows, cols, comps, etc.
//    }
//
//    // Save image data to a file
//    void saveToFile(const std::string& filePath) const {
//        std::ofstream file(filePath, std::ios::binary);
//        if (!file) {
//            throw std::runtime_error("Failed to open file: " + filePath);
//        }
//        file.write(reinterpret_cast<const char*>(data.data()), data.size());
//    }
};

class MosaifyDatabaseTest : public ::testing::Test {
protected:
    MosaifyDatabase db;
    std::string error_message;

    void SetUp() override {
        // Set up the database connection
        std::string connectionString = std::getenv("DB_CONN_STRING");
        ASSERT_TRUE(db.connect(connectionString, error_message)) << "Failed to connect: " << error_message;

//        MosaifyDatabase::executeSQL(db, "CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, name TEXT)", error_message);
//        MosaifyDatabase::executeSQL(db, "TRUNCATE TABLE IF NOT EXISTS test_table", error_message);

        // Optionally reset the database for a clean test environment
        ASSERT_TRUE(db.reset(error_message)) << "Failed to reset database: " << error_message;
    }

    void TearDown() override {
//        MosaifyDatabase::executeSQL(db, "DROP TABLE IF EXISTS test_table", error_message);
        // Disconnect from the database
        db.disconnect();
    }
};

TEST_F(MosaifyDatabaseTest, ExecuteSQL) {
std::string sql = "SELECT version();";
EXPECT_TRUE(db.executeSQL(sql, error_message)) << "Execute SQL failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, CreateAndReadUser) {
    int user_id =-1;
    EXPECT_TRUE(db.createUser("test@example.com", "Test", "User", user_id, error_message)) << "Create user failed: " << error_message;
    EXPECT_TRUE(db.readUser(user_id, error_message)) << "Read user failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, UpdateAndDeleteUser) {
    int user_id =-1;
    EXPECT_TRUE(db.createUser("test@example.com", "Test", "User", user_id, error_message)) << "Create user failed: " << error_message;
    EXPECT_TRUE(db.updateUser(user_id, "new@example.com", "New", "Name", error_message)) << "Update user failed: " << error_message;
    EXPECT_TRUE(db.deleteUser(user_id, error_message)) << "Delete user failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, CreateAndReadProject) {
    int user_id = -1;
    int project_id = -1;
    std::string project_name = "New Project";
    EXPECT_TRUE(db.createUser("test@example.com", "Test", "User", user_id, error_message)) << "Create user failed: " << error_message;
    EXPECT_TRUE(db.createProject(user_id, "New Project", project_id, error_message)) << "Create project failed: " << error_message;

    int user_id_read;
    std::string project_name_read;
    EXPECT_TRUE(db.readProject(project_id, user_id_read, project_name_read, error_message)) << "Read project failed: " << error_message;
    ASSERT_TRUE(project_name == project_name_read);
    ASSERT_TRUE(user_id == user_id_read);
}

TEST_F(MosaifyDatabaseTest, UpdateAndDeleteProject) {
    int user_id = -1;
    int project_id = -1;
    EXPECT_TRUE(db.createUser("test@example.com", "Test", "User", user_id, error_message)) << "Create user failed: " << error_message;
    ASSERT_TRUE(db.createProject(user_id, "New Project", project_id, error_message));
    EXPECT_TRUE(db.updateProject(user_id, "Updated Project", error_message)) << "Update project failed: " << error_message;
    EXPECT_TRUE(db.deleteProject(user_id, error_message)) << "Delete project failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, CreateAndReadMosaicImage) {
    int user_id = -1;
    int project_id = -1;
    int mosaic_image_id = -1;

    EXPECT_TRUE(db.createUser("test@example.com", "Test", "User", user_id, error_message)) << "Create user failed: " << error_message;
    ASSERT_TRUE(db.createProject(user_id, "New Project", project_id, error_message));

    std::unique_ptr<IImageData> mosaic_image = std::make_unique<ImageData>();
    mosaic_image->setRows(100);
    mosaic_image->setCols(100);
    mosaic_image->setComps(3);
    mosaic_image->setData({0, 1, 2, 3, 4});

    EXPECT_TRUE(db.createMosaicImage(project_id, *mosaic_image, mosaic_image_id, error_message)) << "Create mosaic image failed: " << error_message;
    EXPECT_TRUE(db.readMosaicImage(project_id, *mosaic_image, error_message)) << "Read mosaic image failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, UpdateAndDeleteMosaicImage) {
    int user_id = -1;
    int project_id = -1;
    int mosaic_image_id = -1;

    EXPECT_TRUE(db.createUser("test@example.com", "Test", "User", user_id, error_message)) << "Create user failed: " << error_message;
    ASSERT_TRUE(db.createProject(user_id, "New Project", project_id, error_message));

    std::unique_ptr<IImageData> mosaic_image = std::make_unique<ImageData>();
    mosaic_image->setRows(100);
    mosaic_image->setCols(100);
    mosaic_image->setComps(3);
    mosaic_image->setData({0, 1, 2, 3, 4});

    ASSERT_TRUE(db.createMosaicImage(project_id, *mosaic_image, mosaic_image_id, error_message));

    mosaic_image->setRows(200);
    mosaic_image->setCols(200);
    mosaic_image->setComps(4);
    mosaic_image->setData({5, 6, 7, 8, 9});

    EXPECT_TRUE(db.updateMosaicImage(project_id, *mosaic_image, error_message)) << "Update mosaic image failed: " << error_message;
    EXPECT_TRUE(db.deleteMosaicImage(project_id, error_message)) << "Delete mosaic image failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, CreateAndReadImage) {
    /*
     * James Folk - I am working on this test.
     */

    int user_id = -1;
    int project_id = -1;
    int image_id = -1;

    EXPECT_TRUE(db.createUser("test@example.com", "Test", "User", user_id, error_message)) << "Create user failed: " << error_message;
    ASSERT_TRUE(db.createProject(user_id, "New Project", project_id, error_message));













    int width, height, channels;
    unsigned char* imageData = stbi_load("/Users/jamesfolk/Work/PostgresProjectExample/tests/neighbor.png", &width, &height, &channels, 0);

    ASSERT_FALSE (imageData == nullptr);

    std::string output_filename = "/Users/jamesfolk/Work/PostgresProjectExample/tests/out/CreateAndReadImage_before_db_1.png";
    if (!stbi_write_png(output_filename.c_str(), width, height, channels, imageData, width * channels)) {
        std::cerr << "Error writing PNG: " << stbi_failure_reason() << std::endl; // stbi_failure_reason() might not be helpful here, but it's worth a try.
        std::cerr << "Check file path, disk space, and image data." << std::endl;
    }


    std::unique_ptr<IImageData> image_pre = std::make_unique<ImageData>();
    // Calculate the size in bytes
    size_t sizeInBytes = (size_t)width * height * channels;
    image_pre->setRows(height);
    image_pre->setCols(width);
    image_pre->setComps(channels);

    std::vector<unsigned char> fileData(sizeInBytes);
    memcpy(fileData.data(), imageData, sizeInBytes);
    stbi_image_free(imageData); // Free the image data
    image_pre->setData(fileData);

    output_filename = "/Users/jamesfolk/Work/PostgresProjectExample/tests/out/CreateAndReadImage_before_db_2.png";
    if (!stbi_write_png(output_filename.c_str(), image_pre->getCols(), image_pre->getRows(), image_pre->getComps(), fileData.data(), image_pre->getCols() * image_pre->getComps())) {
        std::cerr << "Error writing PNG: " << stbi_failure_reason() << std::endl; // stbi_failure_reason() might not be helpful here, but it's worth a try.
        std::cerr << "Check file path, disk space, and image data." << std::endl;
    }

    EXPECT_TRUE(db.createImage(project_id, std::move(image_pre), image_id, error_message)) << "Create image failed: " << error_message;

    std::unique_ptr<IImageData> image_post = std::make_unique<ImageData>();
    EXPECT_TRUE(db.readImage(image_id, project_id, *image_post, error_message)) << "Read image failed: " << error_message;

    output_filename = "/Users/jamesfolk/Work/PostgresProjectExample/tests/out/CreateAndReadImage_after.png";
    if (!stbi_write_png(output_filename.c_str(), image_post->getCols(), image_post->getRows(), image_post->getComps(), fileData.data(), image_post->getCols() * image_post->getComps())) {
        std::cerr << "Error writing PNG: " << stbi_failure_reason() << std::endl; // stbi_failure_reason() might not be helpful here, but it's worth a try.
        std::cerr << "Check file path, disk space, and image data." << std::endl;
    }
}

TEST_F(MosaifyDatabaseTest, UpdateAndDeleteImage) {
    int user_id =-1;
    int project_id = -1;
    int image_id = -1;

    EXPECT_TRUE(db.createUser("test@example.com", "Test", "User", user_id, error_message)) << "Create user failed: " << error_message;
    ASSERT_TRUE(db.createProject(user_id, "New Project", project_id, error_message));

    std::unique_ptr<IImageData> image = std::make_unique<ImageData>();
    image->setRows(100);
    image->setCols(100);
    image->setComps(3);
    image->setData({0, 1, 2, 3, 4});

    ASSERT_TRUE(db.createImage(project_id, std::move(image), image_id, error_message));

    EXPECT_TRUE(db.updateImage(image_id, "new_image.png", 200, 200, 4, {5, 6, 7, 8, 9}, error_message)) << "Update image failed: " << error_message;
    EXPECT_TRUE(db.deleteImage(image_id, error_message)) << "Delete image failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, CreateAndReadImages) {
    int user_id =-1;
    int project_id = -1;
    int image_id = -1;

    EXPECT_TRUE(db.createUser("test@example.com", "Test", "User", user_id, error_message)) << "Create user failed: " << error_message;
    ASSERT_TRUE(db.createProject(user_id, "New Project", project_id, error_message));

    std::vector<std::unique_ptr<IImageData>> images;
    images.push_back(std::make_unique<ImageData>("image1.png", 100, 100, 3, std::vector<unsigned char>{0, 1, 2, 3, 4}));
    images.push_back(std::make_unique<ImageData>("image2.png", 200, 200, 3, std::vector<unsigned char>{5, 6, 7, 8, 9}));

    std::vector<int> image_ids;
    EXPECT_TRUE(db.createImages(project_id, images, image_ids, error_message)) << "Create images failed: " << error_message;

    std::vector<std::unique_ptr<IImageData>> read_images;
    auto createImageFunc = []() -> std::unique_ptr<IImageData> {
        return std::make_unique<ImageData>();
    };

    EXPECT_TRUE(db.readImages(project_id, read_images, createImageFunc, image_ids, error_message)) << "Read images failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, ReadProjects) {
    int user_id =-1;
    int project_id = -1;
    int image_id = -1;

    EXPECT_TRUE(db.createUser("test@example.com", "Test", "User", user_id, error_message)) << "Create user failed: " << error_message;
    ASSERT_TRUE(db.createProject(user_id, "New Project", project_id, error_message));

    std::vector<int> project_ids;
    EXPECT_TRUE(db.readProjects(user_id, project_ids, error_message)) << "Read projects failed: " << error_message;
}
