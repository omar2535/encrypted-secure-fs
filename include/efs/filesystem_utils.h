#pragma once

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

#include <efs/utils.h>
#include <efs/crypto.h>

#define FILESYSTEM_ROOT_DIR "filesystem"

namespace Efs {
  class FilesystemUtils {
    public:

      /**
       * @brief Creates a directory on the physical operating system
       *
       * @param dirpath: The full directory path to the dir object
       * 1. Creates sha256 hashes for the directory path
       * 2. Assumes the previous directories exists as well
       *    IE:
       *      dirpath: /admin/personal/
       *      creation steps:
       *        /admin/ -> <hash0>
       *        mkdir <hash0>
       *
       *        /admin/personal/ -> <hash1>
       *        mkdir <hash0>/<hash1>
       */
      static void createDir(std::string dirpath) {
        std::vector<std::string> split_string_dirpath = Utils::splitString(dirpath, '/');
        std::string v_previous_dir = "/";
        std::string r_previous_dir = (std::string) std::filesystem::current_path() + FILESYSTEM_ROOT_DIR;
        for (int i=0; i<split_string_dirpath.size(); i++) {
          if (split_string_dirpath[i] == "") continue;
          // create folders up to the last folder
          std::string v_current_dir = v_previous_dir + split_string_dirpath[i] + "/";

          // get the previous directory's hash, if already exists skip
          std::string r_dirname = Crypto::getSha256ForString(v_current_dir);
          std::string r_current_dir = r_previous_dir + r_dirname + "/";
          if (std::filesystem::is_directory((std::filesystem::path) r_current_dir)) {
            // do nothing if already exists
          } else {
            // create the directory
            std::filesystem::create_directory(r_current_dir);
          }

          r_previous_dir = r_current_dir;
          v_previous_dir = v_current_dir;
        }
      }
  };
}
