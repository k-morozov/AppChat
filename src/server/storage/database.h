#ifndef DATABASE_H
#define DATABASE_H

#include "protocol/protocol.h"
#include <deque>
#include <sqlite3.h>
#include "server/log/logger.h"

#include <memory>

class Database
{
public:
    Database(const std::string& _db_name);
    ~Database();

    void save_text_message(text_request_ptr message);
    std::deque<text_response_ptr> load_history(identifier_t roomid);

    void add_logins(std::string login, identifier_t logi_id, std::string password);

    identifier_t get_loginid(std::string login) const;
//    identifier_t registration(std::string login, std::string password);

    identifier_t check_client(std::string login, std::string password) const;

private:
    const std::string db_name = "history.db";
    sqlite3* db_ptr;
    static std::string create_table_history;
    static std::string create_table_logins;

    log4cplus::Logger logger = LOGGER("Database");
};

using database_ptr = std::shared_ptr<Database>;

#endif // DATABASE_H
