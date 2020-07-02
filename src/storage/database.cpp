#include <storage/database.h>


std::string Database::create_table_query = std::string("create table if not exists history ")
        + std::string("(author varchar[") + std::to_string(Block::LoginName) + std::string("], ")
//        + std::string("client_id integer, ")
        + std::string("room_id integer, ")
        + std::string("message varchar[") + std::to_string(Block::TextMessage) + std::string("]);");

Database::Database()
{
    int rc = sqlite3_open(db_name.c_str(), &db_ptr);
    if(rc) {
        std::cout << "Cannot open database " << sqlite3_errmsg(db_ptr) << std::endl;
        sqlite3_close(db_ptr);
    }
    else {
        char* err_msg1 = 0;
        rc = sqlite3_exec(db_ptr, create_table_query.c_str(),
                             [](void*, int, char**, char**){ return 0;},
                             0, &err_msg1);
        if(rc != SQLITE_OK) {
            std::cout<< "SQL error " << err_msg1 << std::endl;
            sqlite3_free(err_msg1);
            sqlite3_close(db_ptr);
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

//    sqlite3_close(db_ptr);
}

std::deque<text_response_ptr> Database::load_history(identifier_t roomid) {
    std::deque<text_response_ptr> history_room;
    bool found = false;
//    sqlite3* db;
    sqlite3_stmt* stmt;

    std::string sql = std::string("select * from history where room_id==")
            + std::to_string(roomid)
            + std::string(";");
//    if (sqlite3_open("data/test.db", &db) != SQLITE_OK) {
//        printf("ERROR: can't open database: %s\n", sqlite3_errmsg(db));
//        sqlite3_close(db);
//    }
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
        //this error handling could be done better, but it works
        printf("ERROR: while performing sql: %s\n", sqlite3_errmsg(db_ptr));
        printf("ret_code = %d\n", ret_code);
    }

    printf("entry %s\n", found ? "found history" : "not found history");

    //release resources
    sqlite3_finalize(stmt);
//    sqlite3_close(db);
    return history_room;
}
