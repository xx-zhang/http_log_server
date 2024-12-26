#ifndef SQLCIPHERDB_H
#define SQLCIPHERDB_H

#include <string>
#include <mutex>
#include <vector>
#include <sqlcipher/sqlite3.h>

class SqlCipherDb {
public:
    SqlCipherDb(const std::string &dbPath, const std::string &password);
    ~SqlCipherDb();

    bool open();
    bool close();
    bool execute(const std::string &sql);
    bool query(const std::string &sql, std::vector<std::vector<std::string>> &results);
    bool insertMultiple(const std::vector<std::string> &sqls);

private:
    std::string m_dbPath;
    std::string m_password;
    sqlite3 *m_db;
    std::mutex m_mutex;
};

#endif // SQLCIPHERDB_H
