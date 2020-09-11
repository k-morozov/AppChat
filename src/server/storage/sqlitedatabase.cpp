#include "sqlitedatabase.h"
#include "log/logger.h"

#include <filesystem>
#include <utility>
#include <string>
#include <cstdlib>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>

#include <stdexcept>

using namespace std::string_literals;
namespace fs = std::filesystem;

namespace Storage
{


SqliteDatabase::SqliteDatabase(DatabaseConfiguration dbConfig)
  : Database(std::move(dbConfig)), db_{nullptr, &sqlite3_close}
{
  BOOST_LOG_TRIVIAL(info) << "SqliteDatabase()";

  if (!fs::exists(dbConfig_.FolderPath)) {
    fs::create_directory(dbConfig_.FolderPath);
    BOOST_LOG_TRIVIAL(info) << "Create dir for appchat: " << dbConfig_.FolderPath;
  }
}


SqliteDatabase::~SqliteDatabase() { }


bool SqliteDatabase::open()
{
  if (!is_open()) {
    sqlite3* db_ptr = nullptr;

    const auto code = sqlite3_open_v2(dbConfig_.ConnectionString.c_str(), &db_ptr,
      SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_URI, nullptr
    );

    db_.reset(db_ptr);

    if (code) {
      BOOST_LOG_TRIVIAL(fatal) << "Cannot open database: " << sqlite3_errmsg(db_.get());
    } else {
      is_open_ = true;
    }
  }

  return is_open();
}


void SqliteDatabase::close()
{
  db_.reset();
  is_open_ = false;
}


bool SqliteDatabase::create_table_history()
{
  return create_table(create_table_history_query_);
}


bool SqliteDatabase::create_table_logins()
{
  return create_table(create_table_logins_query_);
}


bool SqliteDatabase::save_text_message(TextSendData message)
{
  //const auto dt = message->get_datetime();
  const DateTime dt;
  const std::string str_datetime = boost::str(
      boost::format("%1$d-%2$#02d-%3$#02d %4$#02s:%5$#02s:%6$#02s")
      % (static_cast<unsigned int>(dt.year) + 2000u) % static_cast<unsigned int>(dt.month) % static_cast<unsigned int>(dt.day)
      % static_cast<unsigned int>(dt.hours) % static_cast<unsigned int>(dt.month) % static_cast<unsigned int>(dt.seconds)
  );

  const std::string insert_query = 
    "INSERT INTO history VALUES ("s
      + "'"s + message.login + "', "s
      + std::to_string(message.room_id) + ", "s
      + "strftime('%s', '"s + str_datetime + "'), "s
      + "'"s + message.text + "');"s;

  char* err_msg_ptr = nullptr;

  auto code = sqlite3_exec(db_.get(), insert_query.c_str(),
    [](void*, int, char**, char**) { return 0; },
    0, &err_msg_ptr
  );
  const std::unique_ptr<char[], decltype(&sqlite3_free)> err_msg{err_msg_ptr, &sqlite3_free};

  if (code != SQLITE_OK) {
    BOOST_LOG_TRIVIAL(error) << "SQL error save_text_message: " << err_msg.get();
    return false;
  }

  return true;
}


std::deque<TextSendData> SqliteDatabase::get_history(identifier_t roomid) const
{
  const auto query =
    "SELECT author, room_id, datetime(datetime, 'unixepoch'), message "s
    + "FROM history WHERE room_id="s + std::to_string(roomid) + ";"s;
 
  sqlite3_stmt* stmt_ptr = nullptr;

  auto code = sqlite3_prepare_v2(db_.get(), query.c_str(), -1, &stmt_ptr, nullptr);
  
  const std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> stmt{stmt_ptr, &sqlite3_finalize};
  
  if (code != SQLITE_OK) {
    BOOST_LOG_TRIVIAL(error) << "get_history: while compiling sql: " << sqlite3_errmsg(db_.get());
    return {};
  }

  std::deque<TextSendData> history_room;  

  while ((code = sqlite3_step(stmt.get())) == SQLITE_ROW ) {
    const std::string author = static_cast<const char*>(sqlite3_column_blob(stmt.get(), 0));
    const int room_id = sqlite3_column_int(stmt.get(), 1);
    const std::string dt = static_cast<const char*>(sqlite3_column_blob(stmt.get(), 2));
    const std::string message = static_cast<const char*>(sqlite3_column_blob(stmt.get(), 3));

    BOOST_LOG_TRIVIAL(info) << "DB: " << author << " " << room_id << " " << dt << " " << message;

    history_room.push_back(TextSendData{room_id, author, message});
  }
  
  if (code != SQLITE_DONE) {
    BOOST_LOG_TRIVIAL(error) << "get_history: while performing sql: " << sqlite3_errmsg(db_.get()) << " code = " << code;
  }

  return history_room;
}


bool SqliteDatabase::add_logins(std::string login, identifier_t login_id, std::string password)
{
  const auto query =
    "INSERT INTO logins VALUES ("s
    + "'"s + std::move(login) + "', "s
    + std::to_string(login_id) + ", "s
    + "'"s + std::move(password) + "');"s;

  char* err_msg_ptr = nullptr;

  const auto code = sqlite3_exec(db_.get(), query.c_str(),
    [](void*, int, char**, char**) { return 0; },
    0, &err_msg_ptr
  );
  const std::unique_ptr<char[], decltype(&sqlite3_free)> err_msg{err_msg_ptr, &sqlite3_free};

  if (code != SQLITE_OK) {
    BOOST_LOG_TRIVIAL(error) << "SQL error add_logins" << err_msg.get();
    return false;
  }
  
  return true;
}


identifier_t SqliteDatabase::get_loginid(std::string login) const
{
  identifier_t result = -1;

  const auto query =
    "SELECT * FROM logins WHERE login='"s + login + "';"s;
  
  sqlite3_stmt* stmt_ptr = nullptr;

  auto code = sqlite3_prepare_v2(db_.get(), query.c_str(), -1, &stmt_ptr, nullptr);
  
  std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> stmt{stmt_ptr, &sqlite3_finalize};

  if (code != SQLITE_OK) {
    BOOST_LOG_TRIVIAL(error) << "get_loginid: while compiling sql: " << sqlite3_errmsg(db_.get());
    return result;
  }

  while ((code = sqlite3_step(stmt.get())) == SQLITE_ROW) {
    result = sqlite3_column_int(stmt.get(), 1);
  }

  if (code != SQLITE_DONE) {
    BOOST_LOG_TRIVIAL(error) << "get_loginid: while performing sql: " << sqlite3_errmsg(db_.get());
  }

  return result;
}


identifier_t SqliteDatabase::check_client(std::string login, std::string password) const
{
  identifier_t result = -1;

  sqlite3_stmt* stmt_ptr = nullptr;

  const auto query =
    "SELECT * FROM logins WHERE login='" + login + "';";

  auto code = sqlite3_prepare_v2(db_.get(), query.c_str(), -1, &stmt_ptr, nullptr);
  
  std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> stmt{stmt_ptr, &sqlite3_finalize};

  if (code != SQLITE_OK) {
    BOOST_LOG_TRIVIAL(error) << "check_client: while compiling sql: " << sqlite3_errmsg(db_.get());
    return result;
  }

  while ((code = sqlite3_step(stmt.get())) == SQLITE_ROW) {
    if (password == reinterpret_cast<const char*>(sqlite3_column_blob(stmt.get(), 2))) {
      result = sqlite3_column_int(stmt.get(), 1);
    }
  }

  if (code != SQLITE_DONE) {
    BOOST_LOG_TRIVIAL(error) << "check_client: while preforming sql: " << sqlite3_errmsg(db_.get()) << " code = " << code;
  }

  return result;
}


std::string SqliteDatabase::create_table_history_query_ =
  "CREATE TABLE IF NOT EXISTS history ("s
    + "author VARCHAR["s + std::to_string(Block::LoginName) + "], "s
    + "room_id INTEGER, "s
    + "datetime INTEGER, "s
    + "message VARCHAR["s + std::to_string(Block::TextMessage) + "]);"s;


std::string SqliteDatabase::create_table_logins_query_ = 
  "CREATE TABLE IF NOT EXISTS logins ("s
    + "login VARCHAR["s + std::to_string(Block::LoginName) + "], "s
    + "login_id INTEGER, "s
    + "password VARCHAR["s + std::to_string(Block::Password) + "]);"s;


bool SqliteDatabase::create_table(const std::string& query)
{
  char* err_msg_ptr = nullptr;
  
  const auto code = sqlite3_exec(db_.get(), query.c_str(),
    [](void*, int, char**, char**) { return 0; },
    0, &err_msg_ptr
  );
  const std::unique_ptr<char[], decltype(&sqlite3_free)> err_msg{err_msg_ptr, &free};

  if (code != SQLITE_OK) {
    BOOST_LOG_TRIVIAL(error) << "SQL error create_table" << err_msg.get();
    return false;
  }

  return true;
}

} // namespace Storage
