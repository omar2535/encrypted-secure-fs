#pragma once

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

namespace Efs {
  class Filesystem {
    public:
      Filesystem();
      ~Filesystem();

      /* For Db.json */
      void createFile(std::string filepath);
      void updateFile(std::string filepath);
      void deleteFile(std::string filepath);
      void getFile(std::string filepath);

      void createDirectory(std::string dirpath);
      void updateDirectory(std::string dirpath);
      void deleteDirectory(std::string dirpath);

      void doesFileExist(std::string filepath);
      void doesDirectoryExist(std::string dirpath);

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
}
