#include <gtest/gtest.h>
#include "MosaifyDatabase/MosaifyDatabase.h"  // Include your database header
#include "MosaifyDatabase/IImageData.h"

#include <string>
#include <memory>
#include <vector>

#include <map>
#include <fstream>
#include <sstream>

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

    // Load image data from a file
    void loadFromFile(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }
        data.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        filename = filePath;
        // Additional logic to set rows, cols, comps, etc.
    }

    // Save image data to a file
    void saveToFile(const std::string& filePath) const {
        std::ofstream file(filePath, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
    }
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
EXPECT_TRUE(db.createUser("test@example.com", "Test", "User", error_message)) << "Create user failed: " << error_message;
EXPECT_TRUE(db.readUser(1, error_message)) << "Read user failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, UpdateAndDeleteUser) {
ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
EXPECT_TRUE(db.updateUser(1, "new@example.com", "New", "Name", error_message)) << "Update user failed: " << error_message;
EXPECT_TRUE(db.deleteUser(1, error_message)) << "Delete user failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, CreateAndReadProject) {
ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
EXPECT_TRUE(db.createProject(1, "New Project", error_message)) << "Create project failed: " << error_message;
EXPECT_TRUE(db.readProject(1, error_message)) << "Read project failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, UpdateAndDeleteProject) {
ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
ASSERT_TRUE(db.createProject(1, "New Project", error_message));
EXPECT_TRUE(db.updateProject(1, "Updated Project", error_message)) << "Update project failed: " << error_message;
EXPECT_TRUE(db.deleteProject(1, error_message)) << "Delete project failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, CreateAndReadMosaicImage) {
ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
ASSERT_TRUE(db.createProject(1, "New Project", error_message));

std::unique_ptr<IImageData> mosaic_image = std::make_unique<ImageData>();
mosaic_image->setRows(100);
mosaic_image->setCols(100);
mosaic_image->setComps(3);
mosaic_image->setData({0, 1, 2, 3, 4});

EXPECT_TRUE(db.createMosaicImage(1, *mosaic_image, error_message)) << "Create mosaic image failed: " << error_message;
EXPECT_TRUE(db.readMosaicImage(1, *mosaic_image, error_message)) << "Read mosaic image failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, UpdateAndDeleteMosaicImage) {
ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
ASSERT_TRUE(db.createProject(1, "New Project", error_message));

std::unique_ptr<IImageData> mosaic_image = std::make_unique<ImageData>();
mosaic_image->setRows(100);
mosaic_image->setCols(100);
mosaic_image->setComps(3);
mosaic_image->setData({0, 1, 2, 3, 4});

ASSERT_TRUE(db.createMosaicImage(1, *mosaic_image, error_message));

mosaic_image->setRows(200);
mosaic_image->setCols(200);
mosaic_image->setComps(4);
mosaic_image->setData({5, 6, 7, 8, 9});

EXPECT_TRUE(db.updateMosaicImage(1, *mosaic_image, error_message)) << "Update mosaic image failed: " << error_message;
EXPECT_TRUE(db.deleteMosaicImage(1, error_message)) << "Delete mosaic image failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, CreateAndReadImage) {
ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
ASSERT_TRUE(db.createProject(1, "New Project", error_message));

std::unique_ptr<IImageData> image = std::make_unique<ImageData>();
image->setRows(100);
image->setCols(100);
image->setComps(3);
image->setData({0, 1, 2, 3, 4});

EXPECT_TRUE(db.createImage(1, *image, error_message)) << "Create image failed: " << error_message;

int project_id;
EXPECT_TRUE(db.readImage(1, project_id, *image, error_message)) << "Read image failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, UpdateAndDeleteImage) {
ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
ASSERT_TRUE(db.createProject(1, "New Project", error_message));

std::unique_ptr<IImageData> image = std::make_unique<ImageData>();
image->setRows(100);
image->setCols(100);
image->setComps(3);
image->setData({0, 1, 2, 3, 4});

ASSERT_TRUE(db.createImage(1, *image, error_message));

EXPECT_TRUE(db.updateImage(1, "new_image.png", 200, 200, 4, {5, 6, 7, 8, 9}, error_message)) << "Update image failed: " << error_message;
EXPECT_TRUE(db.deleteImage(1, error_message)) << "Delete image failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, CreateAndReadImages) {
ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
ASSERT_TRUE(db.createProject(1, "New Project", error_message));

std::vector<std::unique_ptr<IImageData>> images;
images.push_back(std::make_unique<ImageData>("image1.png", 100, 100, 3, std::vector<unsigned char>{0, 1, 2, 3, 4}));
images.push_back(std::make_unique<ImageData>("image2.png", 200, 200, 3, std::vector<unsigned char>{5, 6, 7, 8, 9}));

EXPECT_TRUE(db.createImages(1, images, error_message)) << "Create images failed: " << error_message;

std::vector<std::unique_ptr<IImageData>> read_images;
auto createImageFunc = []() -> std::unique_ptr<IImageData> {
    return std::make_unique<ImageData>();
};

EXPECT_TRUE(db.readImages(1, read_images, createImageFunc, error_message)) << "Read images failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, ReadProjects) {
ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
ASSERT_TRUE(db.createProject(1, "New Project", error_message));

std::vector<int> project_ids;
EXPECT_TRUE(db.readProjects(1, project_ids, error_message)) << "Read projects failed: " << error_message;
}