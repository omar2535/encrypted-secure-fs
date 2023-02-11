#pragma once

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

namespace Efs {
  class Database {
    public:
      Database();
      ~Database();

      /* For keys.json */
      void addKeypair(std::string username, std::string key);
      void getKeyByUser(std::string username);

      /* For Shared.json */
      std::vector<std::string> getSharedFilesForUser();
      void addSharedFileForUser(std::string username, std::string filepath);

    private:
      std::string DB_FILE = "DB.json";
      std::string SHARED_FILE = "Shared.json";
      std::string KEYS_FILE = "Keys.json";





  };
} // Efs namespace
