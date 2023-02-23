#pragma once

#include <iostream>

namespace Efs {
  class User {
    public:
      User(std::string username);
      std::string getUsername();

    private:
      std::string username;
  };
}
