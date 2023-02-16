#pragma once
#include <string>

namespace Efs{

  class Efs {
    public:
      std::string m_currentDir;
      Efs(int argc, char** argv);
  };
}
