#include <gtest/gtest.h>
#include "MosaifyDatabase.h"
#include <string>

using namespace NJLIC;

class MosaifyDatabaseTest : public ::testing::Test {
protected:
    MosaifyDatabase db;
    std::string error_message;

    void SetUp() override {
        // Set up the database connection
        std::string connectionString = std::getenv("DB_CONN_STRING");
        ASSERT_TRUE(db.connect(connectionString, error_message)) << "Failed to connect: " << error_message;

        // Optionally reset the database for a clean test environment
        ASSERT_TRUE(db.reset(error_message)) << "Failed to reset database: " << error_message;
    }

    void TearDown() override {
        // Disconnect from the database
        db.disconnect();
    }
};

TEST_F(MosaifyDatabaseTest, CreateUser) {
    EXPECT_TRUE(db.createUser("test@example.com", "Test", "User", error_message)) << "Create user failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, ReadUser) {
    ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
    EXPECT_TRUE(db.readUser(1, error_message)) << "Read user failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, UpdateUser) {
    ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
    EXPECT_TRUE(db.updateUser(1, "new@example.com", "New", "Name", error_message)) << "Update user failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, DeleteUser) {
    ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
    EXPECT_TRUE(db.deleteUser(1, error_message)) << "Delete user failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, CreateProject) {
    ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
    EXPECT_TRUE(db.createProject(1, "New Project", error_message)) << "Create project failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, ReadProject) {
    ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
    ASSERT_TRUE(db.createProject(1, "New Project", error_message));
    EXPECT_TRUE(db.readProject(1, error_message)) << "Read project failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, UpdateProject) {
    ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
    ASSERT_TRUE(db.createProject(1, "New Project", error_message));
    EXPECT_TRUE(db.updateProject(1, "Updated Project", error_message)) << "Update project failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, DeleteProject) {
    ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
    ASSERT_TRUE(db.createProject(1, "New Project", error_message));
    EXPECT_TRUE(db.deleteProject(1, error_message)) << "Delete project failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, CreateImage) {
    ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
    ASSERT_TRUE(db.createProject(1, "New Project", error_message));
    std::vector<unsigned char> data = {0, 1, 2, 3, 4};
//    EXPECT_TRUE(db.createImage(1, "image.png", 100, 100, 3, data, error_message)) << "Create image failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, ReadImage) {
    ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
    ASSERT_TRUE(db.createProject(1, "New Project", error_message));
    std::vector<unsigned char> data = {0, 1, 2, 3, 4};
//    ASSERT_TRUE(db.createImage(1, "image.png", 100, 100, 3, data, error_message));
//    EXPECT_TRUE(db.readImage(1, error_message)) << "Read image failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, UpdateImage) {
    ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
    ASSERT_TRUE(db.createProject(1, "New Project", error_message));
    std::vector<unsigned char> data = {0, 1, 2, 3, 4};
//    ASSERT_TRUE(db.createImage(1, "image.png", 100, 100, 3, data, error_message));
    std::vector<unsigned char> new_data = {5, 6, 7, 8, 9};
    EXPECT_TRUE(db.updateImage(1, "new_image.png", 200, 200, 4, new_data, error_message)) << "Update image failed: " << error_message;
}

TEST_F(MosaifyDatabaseTest, DeleteImage) {
    ASSERT_TRUE(db.createUser("test@example.com", "Test", "User", error_message));
    ASSERT_TRUE(db.createProject(1, "New Project", error_message));
    std::vector<unsigned char> data = {0, 1, 2, 3, 4};
//    ASSERT_TRUE(db.createImage(1, "image.png", 100, 100, 3, data, error_message));
    EXPECT_TRUE(db.deleteImage(1, error_message)) << "Delete image failed: " << error_message;
}
