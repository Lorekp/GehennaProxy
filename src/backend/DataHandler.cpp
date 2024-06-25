#include "DataHandler.h"

DataHandler* DataHandler::sInstance = nullptr;

namespace fs = std::filesystem;

DataHandler::DataHandler()
{
	assert(sInstance == nullptr);
	sInstance = this;

    //create "data" folder

    const fs::path exec_path = fs::current_path();

    // Define the new directory name
    const fs::path data_dir = exec_path / "data";

    // Create the directory
    try {
        if (fs::create_directory(data_dir)) {
            std::cout << "Directory created successfully: " << data_dir << std::endl;
        }
        else if (fs::exists(data_dir) && fs::is_directory(data_dir)) {
            std::cout << "Directory already exists: " << data_dir << std::endl;
        }
        else {
            std::cerr << "Failed to create directory: " << data_dir << std::endl;
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }

    //create key database

    fs::path db_path = data_dir / "playerData.db";

    db_path = data_dir / "key.db";

    // Open a database connection

    if (const int exit = sqlite3_open(db_path.string().c_str(), &keyDB); exit != SQLITE_OK) {
        std::cerr << "Can't open key.db database: " << sqlite3_errmsg(keyDB) << std::endl;
    }
    else {
        std::cout << "Opened key.db database successfully" << std::endl;
    }

    // Create an SQL statement
    const auto keySetup = "CREATE TABLE IF NOT EXISTS KEYS("
        "USED INT,"
        "HASH TEXT"
        ");";

    executeSQLStatement(keySetup, keyDB);

    sqlite3_close(keyDB);
}

void DataHandler::executeSQLStatement(const char* statement, sqlite3* database) {
    char* errorMessage = nullptr;

    if (const int exit = sqlite3_exec(database, statement, nullptr, nullptr, &errorMessage); exit != SQLITE_OK) {
        if (errorMessage) {
            std::cerr << "SQL error: " << errorMessage << std::endl;
            sqlite3_free(errorMessage);
        }
        else {
            std::cerr << "SQL error with no error message available." << std::endl;
        }
    }
    else {
        std::cout << "Executed an SQL statement successfully" << std::endl;
    }
}