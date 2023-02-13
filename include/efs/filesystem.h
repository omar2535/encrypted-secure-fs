#pragma once

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

namespace Efs {
  class Filesystem {
    public:
      Filesystem(std::string username, std::string filepath, std::string private_key_filepath);
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

    private:
      nlohmann::json json;
      std::string username;
      std::string filepath;
      std::string private_key_filepath;

      void initializeNewFilesystem();
  };
}
