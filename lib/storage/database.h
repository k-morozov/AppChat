#ifndef DATABASE_H
#define DATABASE_H

#include <protocol.h>
#include <deque>
#include <sqlite3.h>
#include <../log/logger.h>

class Database
{
public:
    static Database& Instance() {
        static Database storage;
        return storage;
    }

    void save_text_message(text_request_ptr message);
    std::deque<text_response_ptr> load_history(identifier_t roomid);

    void add_logins(std::string login, identifier_t logi_id, std::string password);

    identifier_t get_loginid(std::string login) const;
//    identifier_t registration(std::string login, std::string password);

    identifier_t check_client(std::string login, std::string password) const;

    ~Database();
private:
    Database();

    const std::string db_name = "history.db";
    sqlite3* db_ptr;
    static std::string create_table_history;
    static std::string create_table_logins;
};

#endif // DATABASE_H
