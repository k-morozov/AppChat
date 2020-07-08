#include "database.h"

std::string Database::create_table_history = std::string("create table if not exists history ")
        + std::string("(author varchar[") + std::to_string(Block::LoginName) + std::string("], ")
//        + std::string("client_id integer, ")
        + std::string("room_id integer, ")
        + std::string("message varchar[") + std::to_string(Block::TextMessage) + std::string("]);");


std::string Database::create_table_logins = std::string("create table if not exists logins ")
        + std::string("(login varchar[") + std::to_string(Block::LoginName) + std::string("], ")
        + std::string("login_id integer, ")
        + std::string("password varchar[") + std::to_string(Block::Password) + std::string("]);");



Database::Database()
{
    int rc = sqlite3_open(db_name.c_str(), &db_ptr);
    if(rc) {
        std::cout << "Cannot open database " << sqlite3_errmsg(db_ptr) << std::endl;
        sqlite3_close(db_ptr);
    }
    else {
        char* err_msg1 = 0;
        rc = sqlite3_exec(db_ptr, create_table_history.c_str(),
                             [](void*, int, char**, char**){ return 0;},
                             0, &err_msg1);
        if(rc != SQLITE_OK) {
            std::cout<< "SQL error " << err_msg1 << std::endl;
            sqlite3_free(err_msg1);
            sqlite3_close(db_ptr);
        }
        else {
            rc = sqlite3_exec(db_ptr, create_table_logins.c_str(),
                                 [](void*, int, char**, char**){ return 0;},
                                 0, &err_msg1);
            if(rc != SQLITE_OK) {
                std::cout<< "SQL error " << err_msg1 << std::endl;
                sqlite3_free(err_msg1);
                sqlite3_close(db_ptr);
            }
        }
    }
}

Database::~Database() {
    sqlite3_close(db_ptr);
}

void Database::save_text_message(text_request_ptr message) {

    const std::string insert_query = std::string("insert into history values('")
                                      + std::string(message->get_login()) + std::string("', ")
                                      + std::to_string(message->get_roomid()) + std::string(", '")
                                      + std::string(message->get_message()) + std::string("');");
    char* err_msg2 = 0;
    int rc = sqlite3_exec(db_ptr, insert_query.c_str(),
                         [](void*, int, char**, char**){ return 0;},
                         0, &err_msg2);
    if(rc != SQLITE_OK) {
        std::cout << "SQL error " << err_msg2 << std::endl;
        sqlite3_free(err_msg2);
        sqlite3_close(db_ptr);
        return;
    }

}

std::deque<text_response_ptr> Database::load_history(identifier_t roomid) {
    std::deque<text_response_ptr> history_room;
    bool found = false;
    sqlite3_stmt* stmt;

    std::string sql = std::string("select * from history where room_id==")
            + std::to_string(roomid)
            + std::string(";");

    if (sqlite3_prepare_v2(db_ptr, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        printf("ERROR: while compiling sql: %s\n", sqlite3_errmsg(db_ptr));
        sqlite3_close(db_ptr);
        sqlite3_finalize(stmt);
    }
    int ret_code = 0;
    while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::cout << (const char *)sqlite3_column_blob(stmt, 0) << " " << sqlite3_column_int(stmt, 1) << " "
                  << (const char *)sqlite3_column_blob(stmt, 2) << std::endl;
        text_response_ptr response = std::make_shared<TextResponse>(
                (const char *)sqlite3_column_blob(stmt, 0),
                (const char *)sqlite3_column_blob(stmt, 2),
                sqlite3_column_int(stmt, 1));
        history_room.push_back(response);
        found=true;
    }
    if(ret_code != SQLITE_DONE) {
        printf("ERROR: while performing sql: %s\n", sqlite3_errmsg(db_ptr));
        printf("ret_code = %d\n", ret_code);
    }

    printf("entry %s\n", found ? "found history" : "not found history");

    sqlite3_finalize(stmt);
    return history_room;
}

void Database::add_logins(std::string login, identifier_t login_id, std::string password) {
    const std::string insert_query = std::string("insert into logins values('")
                                      + login + std::string("', ")
                                      + std::to_string(login_id) + std::string(", '")
                                      + password + std::string("');");
//    std::cout << insert_query << std::endl;
    char* err_msg2 = 0;
    int rc = sqlite3_exec(db_ptr, insert_query.c_str(),
                         [](void*, int, char**, char**){ return 0;},
                         0, &err_msg2);
    if(rc != SQLITE_OK) {
        std::cout << "SQL error " << err_msg2 << std::endl;
        sqlite3_free(err_msg2);
        sqlite3_close(db_ptr);
        return;
    }
}

identifier_t Database::get_loginid(std::string login) const {
    identifier_t result = -1;
    bool found = false;
    sqlite3_stmt* stmt;

    std::string sql = std::string("select * from logins where login=='")
            + login
            + std::string("';");

    if (sqlite3_prepare_v2(db_ptr, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        printf("ERROR: while compiling sql: %s\n", sqlite3_errmsg(db_ptr));
        sqlite3_close(db_ptr);
        sqlite3_finalize(stmt);
    }
    int ret_code = 0;
    while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::cout << (const char *)sqlite3_column_blob(stmt, 0) << " " << sqlite3_column_int(stmt, 1) << " "
                  << (const char *)sqlite3_column_blob(stmt, 2) << std::endl;
        result = sqlite3_column_int(stmt, 1);
        found = true;
    }
    if(ret_code != SQLITE_DONE) {
        printf("ERROR: while performing sql: %s\n", sqlite3_errmsg(db_ptr));
        printf("ret_code = %d\n", ret_code);
    }

    printf("login %s\n", found ? "found client" : "not found client");

    sqlite3_finalize(stmt);

    return result;
}

identifier_t Database::check_client(std::string login, std::string password) const {
    identifier_t result = -1;
    bool found = false;
    sqlite3_stmt* stmt;

    std::string sql = std::string("select * from logins where login=='")
            + login
            + std::string("';");

    if (sqlite3_prepare_v2(db_ptr, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        printf("ERROR: while compiling sql: %s\n", sqlite3_errmsg(db_ptr));
        sqlite3_close(db_ptr);
        sqlite3_finalize(stmt);
    }
    int ret_code = 0;

    while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::cout << (const char *)sqlite3_column_blob(stmt, 0) << " " << sqlite3_column_int(stmt, 1) << " "
                  << (const char *)sqlite3_column_blob(stmt, 2) << std::endl;
        if ( password == (const char*)sqlite3_column_blob(stmt, 2)) {
            result = sqlite3_column_int(stmt, 1);
        }
        found = true;
    }
    if(ret_code != SQLITE_DONE) {
        printf("ERROR: while performing sql: %s\n", sqlite3_errmsg(db_ptr));
        printf("ret_code = %d\n", ret_code);
    }

    printf("login %s\n", found ? "found client" : "not found client");

    sqlite3_finalize(stmt);

    return result;
}
