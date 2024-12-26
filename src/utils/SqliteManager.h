#ifndef SQLITE_MANAGER_H
#define SQLITE_MANAGER_H

#include <string>
#include <SQLiteCpp/SQLiteCpp.h>
#include "nlohmann/json.hpp"


class DatabaseManager {
public:
    DatabaseManager(const std::string& dbFilename);
    ~DatabaseManager();

    void executeQuery(const std::string& query);
    void selectQuery(const std::string& query, nlohmann::json& json_data);
    void insert(const std::string& table, const std::string& values);
    void update(const std::string& table, const std::string& setClause, const std::string& whereClause);
    void remove(const std::string& table, const std::string& whereClause);
    int info();

private:
    SQLite::Database mDb;
};

#endif // SQLITE_MANAGER_H
