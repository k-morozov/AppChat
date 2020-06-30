#include <database/database.h>


std::string Database::create_table_query = std::string("create table if not exists history ")
        + std::string("(login varchar[") + std::to_string(Block::LoginName) + std::string("], ")
        + std::string("client_id integer, ")
        + std::string("room_id integer, ")
        + std::string("message_body varchar[") + std::to_string(Block::TextMessage) + std::string("]);");

Database::Database()
{

}

void Database::save_to_db(text_response_ptr message) {
    const std::string file_name = "history.db";
    sqlite3* db;
    int rc = sqlite3_open(file_name.c_str(), &db);
    if(rc) {
        std::cout << "Cannot open database " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }

    char* err_msg1 = 0;
    rc = sqlite3_exec(db, create_table_query.c_str(),
                         [](void*, int, char**, char**){ return 0;},
                         0, &err_msg1);
    if(rc != SQLITE_OK) {
        std::cout<< "SQL error " << err_msg1 << std::endl;
        sqlite3_free(err_msg1);
        sqlite3_close(db);
        return;
//        throw ;
    }
    const std::string insert_query = std::string("insert into history values('")
                                      + std::string(message->get_login()) + std::string("', ")
                                      + std::to_string(message->get_roomid()) + std::string(", ")
                                      + std::to_string(message->get_roomid()) + std::string(", '")
                                      + std::string(message->get_message()) + std::string("');");
//    std::cout << "sqlite: " << insert_query << std::endl;
    char* err_msg2 = 0;
    rc = sqlite3_exec(db, insert_query.c_str(),
                         [](void*, int, char**, char**){ return 0;},
                         0, &err_msg2);
    if(rc != SQLITE_OK) {
        std::cout << "SQL error " << err_msg2 << std::endl;
        sqlite3_free(err_msg2);
        sqlite3_close(db);
        return;
    }

    sqlite3_close(db);
}
