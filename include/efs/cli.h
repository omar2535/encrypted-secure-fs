#include <iostream>

namespace Efs {
  class CLI {
    public:
      std::string m_currentDir;
      void cd(std::string currentDir, std::string targetDir);
      void pwd(std::string currentDir);
      void ls(std::string currentDir);
      void cat(std::string currentDir, std::string filepath);
      void share(std::string currentDir, std::string filepath, std::string targetuser);
      void mkdir(std::string currentDir, std::string dirpath);
      void mkfile(std::string currentDir, std::string filepath, std::string contents);
      void exit();
  };
}
