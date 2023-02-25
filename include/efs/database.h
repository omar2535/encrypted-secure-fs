#pragma once

#include <iostream>
#include <fstream>
#include <map>

#include <efs/crypto.h>
#include <nlohmann/json.hpp>

namespace Efs {
  class Database {
    public:
      Database();
      ~Database();

      /* Checks if DB is intiialized*/
      bool isDatabaseInitialized();
      void initializeDatabase();

      /* For File_mappings.json */
      std::vector<std::string> getAllFilePaths();
      std::string addFile(std::string filepath);
      std::string addDir(std::string dirpath);
      void deleteFile(std::string filepath);
      std::string getFilepathFromSha256(std::string hash_string);
      std::string getSha256FromFilePath(std::string filepath);
      bool doesFileExist(std::string v_filepath);
      bool doesDirExist(std::string v_dirpath);


      /* For Users_info.json */
      void createUser(std::string username);
      bool doesUserExist(std::string username);
      std::string getPublicKeyForUser(std::string username);
      std::string getUsernameByPrivateKey(std::string private_key);
      std::string getPropertyForUserInfo(std::string username, std::string property);

      /* For Shared.json */
      void addSharedFileForFile(std::string src_filepath, std::string dst_filepath);
      std::vector<std::string> getSharedFilesForFile(std::string src_filepath);

    private:
      std::string FILE_MAPPINGS_FILE = "File_mappings.json";
      std::string SHARED_FILE = "Shared.json";
      std::string USER_INFO_FILE = "Users_info.json";

      nlohmann::json file_mappings_json;
      nlohmann::json shared_json;
      nlohmann::json user_info_json;

      std::map<std::string, nlohmann::json*> db_files_map = {
        {this->FILE_MAPPINGS_FILE, &(this->file_mappings_json)},
        {this->SHARED_FILE, &(this->shared_json)},
        {this->USER_INFO_FILE, &(this->user_info_json)}
      };


      /* For functions to call */
      void saveDbState();
  };
} // Efs namespace
