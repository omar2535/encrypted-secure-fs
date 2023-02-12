#pragma once

#include <iostream>
#include "efs/database.h"
#include "efs/filesystem.h"
#include "efs/crypto.h"

namespace Efs {
  class Admin {
    public:
      std::string username = "Admin";

      void initilizeAdmin();

    


    private:
      void initializeAdminFilesystem();
      void initializeAdminKeyPair();
  };
}
