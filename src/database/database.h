#ifndef DATABASE_H
#define DATABASE_H

#include <protocol/protocol.h>
#include <sqlite3.h>

class Database
{
public:
    static Database Instance() {
        static Database database;
        return database;
    }
    void startup_db();
    void save_to_db(text_response_ptr message);
private:
    Database();
    static std::string create_table_query;
};

#endif // DATABASE_H
