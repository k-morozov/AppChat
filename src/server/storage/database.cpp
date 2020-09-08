#include "database_.h"

#include <utility>

namespace Storage
{

Database::Database(DatabaseConfiguration dbConfig)
  : dbConfig_{dbConfig}, is_open_{false}
{ }

Database::~Database() { }

bool Database::is_open() { return is_open_; }

} // namespace Storage
