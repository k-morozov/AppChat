#pragma once

#include <string>

namespace Storage
{

struct DatabaseConfiguration
{
  std::string ConnectionString;
  std::string FolderPath;
};

} // namespace Storage
