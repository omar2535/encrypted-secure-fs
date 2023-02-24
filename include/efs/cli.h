#include <iostream>
#include <iostream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <fstream>

#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#include <efs/database.h>
#include <efs/utils.h>
#include <efs/user_manager.h>
#include <efs/filesystem_service.h>

namespace Efs {
  class CLI {
    public:
      std::string v_current_dir;
      std::string m_currentDir; // TODO: Remove this. Only keep v_current_dir

      /**
       * @brief Construct a new CLI object
       *
       * @param database database pointer
       */
      CLI(Database* database, std::string username, std::string initial_dir);

      std::vector<std::string> cd(std::string currentDir, std::string targetDir);
      void pwd(std::string currentDir);
      void ls(std::string currentDir);
      void cat(std::string currentDir, std::string filepath);
      void share(std::string currentDir, std::string filepath, std::string targetuser);
      void mkdir(std::string currentUser, std::string r_currentDir,
                 std::string v_currentDir, std::string v_dirname);
      void mkfile(std::string v_filename, std::string contents);
      void adduser(std::string username);
      void exit();

    private:
      FilesystemService filesystem_service;
      Database* database;
      std::string username;
  };
}
