#include "sqlitex.h"

SqlCipherDb::SqlCipherDb(const std::string &dbPath, const std::string &password) :
    m_dbPath(dbPath), m_password(password), m_db(nullptr) {
}

SqlCipherDb::~SqlCipherDb() {
    close();
}

bool SqlCipherDb::open() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (sqlite3_open(m_dbPath.c_str(), &m_db) != SQLITE_OK) {
        return false;
    }

    std::string key = "PRAGMA key = '" + m_password + "';";
    if (sqlite3_exec(m_db, key.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        sqlite3_close(m_db);
        m_db = nullptr;
        return false;
    }

    return true;
}

bool SqlCipherDb::close() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_db != nullptr) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }

    return true;
}

bool SqlCipherDb::execute(const std::string &sql) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_db == nullptr) {
        return false;
    }

    if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        return false;
    }

    return true;
}

bool SqlCipherDb::query(const std::string &sql, std::vector<std::vector<std::string>> &results) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_db == nullptr) {
        return false;
    }

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    int columnCount = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::vector<std::string> row;
        for (int i = 0; i < columnCount; ++i) {
            row.emplace_back(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
        }
        results.push_back(row);
    }

    sqlite3_finalize(stmt);
    return true;
}

bool SqlCipherDb::insertMultiple(const std::vector<std::string> &sqls) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_db == nullptr) {
        return false;
    }

    char *errorMessage = nullptr;
    if (sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        sqlite3_free(errorMessage);
        return false;
    }

    for (const auto &sql : sqls) {
        if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errorMessage) != SQLITE_OK) {
            sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
            sqlite3_free(errorMessage);
            return false;
        }
    }

    if (sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        sqlite3_free(errorMessage);
        return false;
    }

    return true;
}
