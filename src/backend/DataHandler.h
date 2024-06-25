#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <map>
#include <cassert>
#include <sqlite3.h>
#include <iostream>
#include <filesystem>

class DataHandler {

public:
	DataHandler();

private:

	sqlite3* keyDB{};

	static DataHandler* sInstance;

	static void executeSQLStatement(const char* statement, sqlite3* database);
};

#endif // !DATAHANDLER_H

