#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <filesystem>

#include <efs/cli.h>
#include <efs/database.h>
#include <efs/login.h>
#include <efs/utils.h>
#include <efs/user_manager.h>
#include <efs/filesystem_service.h>

namespace Efs{

  class Efs {
    public:
      std::string m_currentDir;

      Efs(int argc, char** argv);
  };
}
