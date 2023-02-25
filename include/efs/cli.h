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
      std::string private_key;

      /**
       * @brief Construct a new CLI object
       *
       * @param database database pointer
       */
      CLI(Database* database, std::string username, std::string private_key, std::string initial_dir);

      void cd(std::string targetDir);
      void cat(std::string filename);
      void pwd();
      void ls(std::string currentDir);
      void share(std::string filename, std::string target_user);
      void mkdir(std::string dirname);
      void mkfile(std::string filename, std::string contents);
      void adduser(std::string username);
      void exit();

    private:
      FilesystemService filesystem_service;
      Database* database;
      std::string username;
  };
}
