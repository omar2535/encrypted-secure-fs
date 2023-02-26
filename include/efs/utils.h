#pragma once

#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

namespace Efs {
  class Utils {
    public:
      // Gets filename from filepath
      static std::string getFilenameFromFilepath(std::string filepath) {
        if (filepath.back() == '/') {
          filepath.pop_back();
          return filepath.substr(filepath.find_last_of("/\\") + 1) + "/";
        } else {
          return filepath.substr(filepath.find_last_of("/\\") + 1);
        }
      }

      // joins a vector of string by the delimiter given
      static std::string join(std::vector<std::string> const &strings, std::string delim) {
        std::stringstream ss;
        std::copy(strings.begin(), strings.end(), std::ostream_iterator<std::string>(ss, delim.c_str()));
        return ss.str();
      }

      // splits a string by a character into a vector
      static std::vector<std::string> splitString(std::string input, char delim) {
        std::stringstream ss_input(input);
        std::string segment;
        std::vector<std::string> seglist;
        while(std::getline(ss_input, segment, delim)) {
          if (!segment.empty()) seglist.push_back(segment);
        }
        return seglist;
      }

      /**
       * @brief Get the Owner Of Path
       * IE: /admin/asdf -> admin
       * IE: /randomuser/mydir/file.txt -> randomuser
       * @param v_path the virtual path
       * @return std::string returns the username for the owner
       */
      static std::string getOwnerOfPath(std::string v_path) {
        std::vector<std::string> split_path = Utils::splitString(v_path, '/');
        for (auto name : split_path) {
          if (name != "") return name;
        }
        return "";
      }

      /**
       * @brief Determines if the v_path is in the personal folder
       *
       * @param v_path the virtual path
       * @return true if they are in their personal directory </usera/personal/>
       * @return false if they are not </usera/shared>
       */
      static bool isInPersonal(std::string v_path, std::string username) {
        std::vector<std::string> split_path = Utils::splitString(v_path, '/');
        return split_path.size() >= 2 && split_path[1] == "personal" && username == split_path[0];
      }
  };
}
