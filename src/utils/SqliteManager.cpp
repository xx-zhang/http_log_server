#include "SqliteManager.h"

#include <iostream>
#include <chrono>
#include <openssl/lhash.h>
#include <string>

#include "nlohmann/json.hpp"

DatabaseManager::DatabaseManager(const std::string& dbFilename)
    : mDb(dbFilename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
    std::cout << "Database '" << dbFilename << "' opened successfully.\n";
}

DatabaseManager::~DatabaseManager() {}

void DatabaseManager::executeQuery(const std::string& query) {
    try {
        auto start = std::chrono::high_resolution_clock::now();
        int affectedRows = mDb.exec(query);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "Query executed: " << query << "\n";
        std::cout << "Affected rows: " << affectedRows << "\n";
        std::cout << "Elapsed time: " << elapsed.count() << " seconds\n";
    } catch (const std::exception& e) {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
    }
}

void DatabaseManager::selectQuery(const std::string& query, nlohmann::json& json_data) {
    json_data["query"] = query; 
    auto result_data =  nlohmann::json::array(); 
    try {
        SQLite::Statement stmt(mDb, query);
        auto start = std::chrono::high_resolution_clock::now();

        while (stmt.executeStep()) {
            auto raw_dict = new nlohmann::json(); 
            for (int i = 0; i < stmt.getColumnCount(); ++i) {
                (*raw_dict)[stmt.getColumnName(i)] = stmt.getColumn(i); 
            }
            result_data.push_back(*raw_dict); 
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = end - start;

        json_data["results"] = result_data; 
        json_data["code"] = "0"; 
        json_data["elapsed"] = elapsed.count(); 

    } catch (const std::exception& e) {
        json_data["code"] = "-1";
        json_data["elapsed"] = nullptr;
        json_data["results"] = e.what(); 
    }
}

void DatabaseManager::insert(const std::string& table, const std::string& values) {
    std::string query = "INSERT INTO " + table + " VALUES (" + values + ")";
    executeQuery(query);
}

void DatabaseManager::update(const std::string& table, const std::string& setClause, const std::string& whereClause) {
    std::string query = "UPDATE " + table + " SET " + setClause + " WHERE " + whereClause;
    executeQuery(query);
}

void DatabaseManager::remove(const std::string& table, const std::string& whereClause) {
    std::string query = "DELETE FROM " + table + " WHERE " + whereClause;
    executeQuery(query);
}

int DatabaseManager::info() {
    // Implementation for info function (currently empty)

// Using SQLITE_VERSION would require #include <sqlite3.h> which we want to avoid: use SQLite::VERSION if possible.
//  std::cout << "SQlite3 version " << SQLITE_VERSION << std::endl;
    std::cout << "SQlite3 compile time header version " << SQLite::VERSION << " (vs dynamic lib version " << SQLite::getLibVersion() << ")" << std::endl;
    std::cout << "SQliteC++ version " << SQLITECPP_VERSION << std::endl;

    ////////////////////////////////////////////////////////////////////////////
    // Inspect a database via SQLite header information
    try
    {
       const SQLite::Header header = SQLite::Database::getHeaderInfo(mDb.getFilename());

       // Print values for all header fields
       // Official documentation for fields can be found here: https://www.sqlite.org/fileformat.html#the_database_header
        std::cout << "Magic header string: " << header.headerStr << std::endl;
        std::cout << "Page size bytes: " << header.pageSizeBytes << std::endl;
        std::cout << "File format write version: " << (int)header.fileFormatWriteVersion << std::endl;
        std::cout << "File format read version: " << (int)header.fileFormatReadVersion << std::endl;
        std::cout << "Reserved space bytes: " << (int)header.reservedSpaceBytes << std::endl;
        std::cout << "Max embedded payload fraction " << (int)header.maxEmbeddedPayloadFrac << std::endl;
        std::cout << "Min embedded payload fraction: " << (int)header.minEmbeddedPayloadFrac << std::endl;
        std::cout << "Leaf payload fraction: " << (int)header.leafPayloadFrac << std::endl;
        std::cout << "File change counter: " << header.fileChangeCounter << std::endl;
        std::cout << "Database size pages: " << header.databaseSizePages << std::endl;
        std::cout << "First freelist trunk page: " << header.firstFreelistTrunkPage << std::endl;
        std::cout << "Total freelist trunk pages: " << header.totalFreelistPages << std::endl;
        std::cout << "Schema cookie: " << header.schemaCookie << std::endl;
        std::cout << "Schema format number: " << header.schemaFormatNumber << std::endl;
        std::cout << "Default page cache size bytes: " << header.defaultPageCacheSizeBytes << std::endl;
        std::cout << "Largest B tree page number: " << header.largestBTreePageNumber << std::endl;
        std::cout << "Database text encoding: " << header.databaseTextEncoding << std::endl;
        std::cout << "User version: " << header.userVersion << std::endl;
        std::cout << "Incremental vaccum mode: " << header.incrementalVaccumMode << std::endl;
        std::cout << "Application ID: " << header.applicationId << std::endl;
        std::cout << "Version valid for: " << header.versionValidFor << std::endl;
        std::cout << "SQLite version: " << header.sqliteVersion << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << "SQLite exception: " << e.what() << std::endl;
        return EXIT_FAILURE; // unexpected error : exit the example program
    }

    return 0; 
}
