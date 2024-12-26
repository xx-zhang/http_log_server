#include <iostream>
#include <string>
#include <lmdb.h>
#include "lmdb++.h"

class LMDBManager {
public:
    explicit LMDBManager(const std::string &lmdb_data_dir) :
        env(nullptr) {
        env = lmdb::env::create();
        env.set_mapsize(1UL * 1024UL * 1024UL * 1024UL); // 1 GiB
        env.open(lmdb_data_dir.c_str(), 0, 0664);
    }

    ~LMDBManager() {
        env.close();
    }

    bool put(const std::string &key, const std::string &value) {
        try {
            auto txn = lmdb::txn::begin(env);
            auto dbi = lmdb::dbi::open(txn);
            dbi.put(txn, key, value);
            txn.commit();
            return true;
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }

    std::string get(const std::string &key) {
        try {
            auto txn = lmdb::txn::begin(env, nullptr, MDB_RDONLY);
            auto dbi = lmdb::dbi::open(txn);
            std::string result;
            auto fetch = dbi.get(txn, key, result);
            txn.commit();
            return result;
        } catch (const lmdb::not_found_error &e) {
            return "";
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return "";
        }
    }

    bool del(const std::string &key) {
        try {
            auto txn = lmdb::txn::begin(env);
            auto dbi = lmdb::dbi::open(txn);
            bool success = dbi.del(txn, key);
            txn.commit();
            return success;
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }

private:
    lmdb::env env;
};
