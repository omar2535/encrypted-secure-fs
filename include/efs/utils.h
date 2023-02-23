#pragma once

#include <iostream>
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
  };
}
