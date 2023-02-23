#pragma once

#include <iostream>

#include <efs/database.h>
#include <efs/utils.h>
#include <efs/filesystem_utils.h>

namespace Efs {
  class UserManager {
    public:
      UserManager(Database* database);
      std::string createUser(std::string username);

    private:
      Database* database;
  };
}
