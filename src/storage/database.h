#ifndef DATABASE_H
#define DATABASE_H

#include <protocol/protocol.h>
#include <deque>
#include <sqlite3.h>

class Database
{
public:
    static Database& Instance() {
        static Database storage;
        return storage;
    }

    void save_text_message(text_request_ptr message);
    std::deque<text_response_ptr> load_history(identifier_t roomid);

    ~Database();
private:
    Database();

    const std::string db_name = "history.db";
    sqlite3* db_ptr;
    static std::string create_table_query;
};

#endif // DATABASE_H
