#include "database.h"
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>
#include <cstdlib>

std::string Database::create_table_history = std::string("create table if not exists history ")
        + std::string("(author varchar[") + std::to_string(Block::LoginName) + std::string("], ")
//        + std::string("client_id integer, ")
        + std::string("room_id integer, ")
        + std::string("datetime integer, ")
        + std::string("message varchar[") + std::to_string(Block::TextMessage) + std::string("]);");


std::string Database::create_table_logins = std::string("create table if not exists logins ")
        + std::string("(login varchar[") + std::to_string(Block::LoginName) + std::string("], ")
        + std::string("login_id integer, ")
        + std::string("password varchar[") + std::to_string(Block::Password) + std::string("]);");


/**
 * @brief Database::Database
 * @todo add checks for create_directory
 * @todo add open/close db methods
 */
Database::Database() {
    BOOST_LOG_TRIVIAL(info) << "Database()";
    const auto dir_path = std::string(std::getenv("HOME")) + "/Appchat/";
    BOOST_LOG_TRIVIAL(info) << "Home dir: " << dir_path;

    if (!boost::filesystem::exists(dir_path)) {
        boost::filesystem::create_directory(dir_path);
        BOOST_LOG_TRIVIAL(info) << "create dir for appchat: " << dir_path;
    }

    db_name = "file://" + dir_path + "history.db";
    int rc = sqlite3_open_v2(db_name.c_str(), &db_ptr, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_URI, NULL);
    if(rc) {
        BOOST_LOG_TRIVIAL(fatal) << "Cannot open database " << sqlite3_errmsg(db_ptr);
        sqlite3_close(db_ptr);
        db_ptr = NULL;
    }
    else {
        BOOST_LOG_TRIVIAL(info) << "Database has been successfully opened";
        char* err_msg1 = nullptr;
        rc = sqlite3_exec(db_ptr, create_table_history.c_str(),
                             [](void*, int, char**, char**){ return 0;},
                             0, &err_msg1);
        if(rc != SQLITE_OK) {
            BOOST_LOG_TRIVIAL(error) << "SQL error create_table_history" << err_msg1;
            if (err_msg1) sqlite3_free(err_msg1);
            sqlite3_close(db_ptr);
            db_ptr = NULL;
        }
        else {
            if (err_msg1) sqlite3_free(err_msg1);

            rc = sqlite3_exec(db_ptr, create_table_logins.c_str(),
                                 [](void*, int, char**, char**){ return 0;},
                                 0, &err_msg1);
            if(rc != SQLITE_OK) {
                BOOST_LOG_TRIVIAL(error) << "SQL error create_table_logins" << err_msg1;
                if (err_msg1) sqlite3_free(err_msg1);
                sqlite3_close(db_ptr);
                db_ptr = NULL;
            }

        }
    }   // finish open

}

Database::~Database() {
    BOOST_LOG_TRIVIAL(info) << "~Database()";
    sqlite3_close(db_ptr);
    db_ptr = NULL;
}

void Database::save_text_msg(TextSendData msg) {
//    Datetime dt(); // message->get_datetime();
    const std::string str_datetime = boost::str(boost::format("%1$d-%2$#02d-%3$#02d %4$#02s:%5$#02s:%6$#02s")
      % (/*dt.year*/0 + 2000) % static_cast<unsigned int>(/*dt.month*/1) % static_cast<unsigned int>(/*dt.day*/1)
      % static_cast<unsigned int>(9/*dt.hours*/) % static_cast<unsigned int>(15/*dt.minutes*/) % static_cast<unsigned int>(12/*dt.seconds*/)
    );

    const std::string insert_query = std::string("insert into history values('")
                                      + msg.login + std::string("', ")
                                      + std::to_string(msg.room_id) + std::string(", strftime('%s','")
                                      + str_datetime + std::string("'), '")
                                      + msg.text + std::string("');");
    char* err_msg2 = nullptr;
    int rc = sqlite3_exec(db_ptr, insert_query.c_str(),
                         [](void*, int, char**, char**){ return 0;},
                         0, &err_msg2);
    if(rc != SQLITE_OK) {
        BOOST_LOG_TRIVIAL(error) << "SQL error save_text_message" << err_msg2;
        if (err_msg2) sqlite3_free(err_msg2);
        sqlite3_close(db_ptr);
        db_ptr = NULL;
        return;
    }
}

std::deque<TextSendData> Database::get_history(identifier_t roomid) {
    std::deque<TextSendData> history_room;
    bool found = false;
    sqlite3_stmt* stmt;

    std::string sql = std::string("select author, room_id, datetime(datetime, 'unixepoch'), message from history where room_id==")
            + std::to_string(roomid)
            + std::string(";");

    if (sqlite3_prepare_v2(db_ptr, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        BOOST_LOG_TRIVIAL(error) << "ERROR: while compiling sql: " << sqlite3_errmsg(db_ptr);
        sqlite3_finalize(stmt);
        sqlite3_close(db_ptr);
        db_ptr = NULL;

    }

    int ret_code = 0;
    while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {
//        std::string author = (const char *)sqlite3_column_blob(stmt, 0);
        std::string author = reinterpret_cast<const char *>(sqlite3_column_blob(stmt, 0));

        int room_id = sqlite3_column_int(stmt, 1);
//        std::string dt = (const char *)sqlite3_column_blob(stmt, 2);
//        std::string message = (const char *)sqlite3_column_blob(stmt, 3);
        std::string dt = reinterpret_cast<const char *>(sqlite3_column_blob(stmt, 2));
        std::string message = reinterpret_cast<const char *>(sqlite3_column_blob(stmt, 3));

        BOOST_LOG_TRIVIAL(info) <<"Db: " << author << " " << room_id << " " << dt << " " << message;
//        text_response_ptr response = std::make_shared<TextResponse>(author, DateTime(boost::posix_time::time_from_string(dt)), message, room_id);
        // @todo return DateTime
        history_room.push_back(TextSendData{room_id, author, message});
        found=true;
    }
    if(ret_code != SQLITE_DONE) {
        BOOST_LOG_TRIVIAL(info) << "ERROR: while performing sql: " << sqlite3_errmsg(db_ptr)
                                 << " ret_code = " << ret_code;
    }

    BOOST_LOG_TRIVIAL(info) << "entry " << (found ? "found history" : "not found history");
    sqlite3_finalize(stmt);

    return history_room;
}

void Database::add_logins(std::string login, identifier_t login_id, std::string password) {
    const std::string insert_query = std::string("insert into logins values('")
                                      + login + std::string("', ")
                                      + std::to_string(login_id) + std::string(", '")
                                      + password + std::string("');");
    char* err_msg2 = nullptr;
    int rc = sqlite3_exec(db_ptr, insert_query.c_str(),
                         [](void*, int, char**, char**){ return 0;},
                         0, &err_msg2);
    if(rc != SQLITE_OK) {
        BOOST_LOG_TRIVIAL(error) << "SQL error add_logins" << err_msg2;
        if (err_msg2) sqlite3_free(err_msg2);
    }
    if (err_msg2 && db_ptr) sqlite3_free(err_msg2);
}

identifier_t Database::get_loginid(std::string login) const {
    identifier_t result = -1;
    bool found = false;
    sqlite3_stmt* stmt;

    std::string sql = std::string("select * from logins where login=='")
            + login
            + std::string("';");

    if (sqlite3_prepare_v2(db_ptr, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        BOOST_LOG_TRIVIAL(error) << "ERROR: while compiling sql: " << sqlite3_errmsg(db_ptr);
        sqlite3_finalize(stmt);
    }

    int ret_code = 0;
    while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {
//        BOOST_LOG_TRIVIAL(info) << (const char *)sqlite3_column_blob(stmt, 0) << " " << sqlite3_column_int(stmt, 1) << " "
//                                 << (const char *)sqlite3_column_blob(stmt, 2);
        result = sqlite3_column_int(stmt, 1);
        found = true;
    }
    if(ret_code != SQLITE_DONE) {
        BOOST_LOG_TRIVIAL(error) << "ERROR: while performing sql: " << sqlite3_errmsg(db_ptr)
                                 << " ret_code = " << ret_code;
    }

    BOOST_LOG_TRIVIAL(info) << "login " << (found ? "found client" : "not found client");
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
        BOOST_LOG_TRIVIAL(error) << "ERROR: while compiling sql: " << sqlite3_errmsg(db_ptr);
        sqlite3_finalize(stmt);
    }
    int ret_code = 0;

    while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {
//        BOOST_LOG_TRIVIAL(info) << (const char *)sqlite3_column_blob(stmt, 0) << " " << sqlite3_column_int(stmt, 1) << " "
//                                 << (const char *)sqlite3_column_blob(stmt, 2);
        if ( password == reinterpret_cast<const char*>(sqlite3_column_blob(stmt, 2))) {
            result = sqlite3_column_int(stmt, 1);
        }
        found = true;
    }
    if(ret_code != SQLITE_DONE) {
        BOOST_LOG_TRIVIAL(error) << "ERROR: while performing sql: " << sqlite3_errmsg(db_ptr)
                                 << " ret_code = " << ret_code;
    }

    BOOST_LOG_TRIVIAL(info) << "login " << (found ? "found client" : "not found client");
    sqlite3_finalize(stmt);

    return result;
}
