#include <iostream>
#include <cstdlib>  // For std::getenv
#include <vector>
#include "MosaifyDatabase/MosaifyDatabase.h"

int main(int argc, char* argv[]) {
    std::string error_msg;
    // Retrieve the connection string from the environment variable
    const char* conninfo = std::getenv("DB_CONN_STRING");
    if (!conninfo) {
        std::cerr << "Environment variable DB_CONN_STRING is not set." << std::endl;
        return 1;
    }

    NJLIC::MosaifyDatabase *db = new NJLIC::MosaifyDatabase();

    if(!db->connect(conninfo, error_msg)) {
        std::cerr << error_msg;
        return 1;
    }

    bool reset = true;
    // Check for the --reset command-line argument
    if (argc > 1 && std::string(argv[1]) == "--reset") {
        reset = true;
    }

    if(!db->createTables(reset, error_msg)) {
        std::cerr << error_msg << std::endl;
        return 1;
    }


    delete db;

    return 0;
}
