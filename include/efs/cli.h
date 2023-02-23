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

namespace Efs {
  class CLI {
    public:
      std::string m_currentDir;
      std::string v_currentDir;
      std::vector<std::string> cd(std::string currentDir, std::string targetDir, Database* database);
      void pwd(std::string currentDir, Database* database);
      void ls(std::string currentDir, Database* database);
      void cat(std::string currentDir, std::string filepath);
      void share(std::string currentDir, std::string filepath, std::string targetuser);
      void mkdir(std::string currentUser, std::string r_currentDir,
                 std::string v_currentDir, std::string v_dirname,
                 Database* database);
      void mkfile(std::string currentUser, std::string r_currentDir,
                  std::string v_currentDir, std::string v_filename,
                  std::string contents, Database* database);
      void exit();
  };
}
