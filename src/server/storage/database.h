#ifndef DATABASE_H
#define DATABASE_H

#include <protocol/protocol.h>
#include <deque>
#include <sqlite3.h>
#include "log/logger.h"
#include <memory>
#include <boost/filesystem.hpp>

class Database
{
public:
    Database();
    ~Database();

    void save_text_message(text_request_ptr message);
    std::deque<text_response_ptr> get_history(identifier_t roomid);

    void add_logins(std::string login, identifier_t logi_id, std::string password);

    identifier_t get_loginid(std::string login) const;

    identifier_t check_client(std::string login, std::string password) const;

private:
    std::string db_name;
    sqlite3* db_ptr = NULL;
    static std::string create_table_history;
    static std::string create_table_logins;

};

using database_ptr = std::shared_ptr<Database>;

#endif // DATABASE_H
