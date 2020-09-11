#pragma once

#include <deque>
#include <memory>

#include "databaseconfiguration.h"
#include "protocol/protocol.h"

namespace Storage
{

class Database
{
public:
  Database(DatabaseConfiguration dbConfig);
  virtual ~Database();

  virtual bool open() = 0;
  virtual void close() = 0;

  bool is_open();

  virtual bool create_table_history() = 0;
  virtual bool create_table_logins() = 0;

  virtual bool save_text_message(TextSendData message) = 0;
  
  virtual std::deque<TextSendData> get_history(identifier_t roomid) const = 0;

  virtual bool add_logins(std::string login, identifier_t logi_id, std::string password) = 0;

  virtual identifier_t get_loginid(std::string login) const = 0;

  virtual identifier_t check_client(std::string login, std::string password) const = 0;

protected:
  DatabaseConfiguration dbConfig_;
  bool is_open_;
};

using database_ptr = std::shared_ptr<Database>;

} //namespace Storage
