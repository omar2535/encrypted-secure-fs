#pragma once

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

namespace Efs {
  class Database {
    public:
      Database();
      ~Database();

      /* Checks if DB is intiialized*/
      bool isDatabaseInitialized();
      void initializeDatabase();

      /* For keys.json */
      void addKeyToUser(std::string username, std::string key);
      void getKeyByUser(std::string username);
      void storeDbFileForUser(std::string username, std::string db_file);

      /* For Shared.json */
      std::vector<std::string> getSharedFilesForUser();
      void addSharedFileForUser(std::string username, std::string filepath);

    private:
      std::string SHARED_FILE = "Shared.json";
      std::string USER_INFO_FILE = "user_info.json";
  };
} // Efs namespace
