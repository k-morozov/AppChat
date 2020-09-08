#pragma once

#include <sqlite3.h>
#include <memory>

#include "database.h"

namespace Storage
{

class SqliteDatabase : public Database
{
public:
  SqliteDatabase(DatabaseConfiguration dbConfig);
  virtual ~SqliteDatabase();

  virtual bool open() override;
  virtual void close() override;

  virtual bool create_table_history() override;
  virtual bool create_table_logins() override;

  virtual bool save_text_message(TextSendData message) override;
  
  virtual std::deque<TextSendData> get_history(identifier_t roomid) const override;

  virtual bool add_logins(std::string login, identifier_t login_id, std::string password) override;

  virtual identifier_t get_loginid(std::string login) const override;

  virtual identifier_t check_client(std::string login, std::string password) const override;

private:
  std::unique_ptr<sqlite3, decltype(&sqlite3_close_v2)> db_;
  static std::string create_table_history_query_;
  static std::string create_table_logins_query_;

  bool create_table(const std::string& query);
};

} // namespace Storage
