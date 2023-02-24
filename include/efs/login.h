#include <iostream>
#include <fstream>
#include <efs/database.h>
#include <string>
#include <tuple>

namespace Efs {
  static std::tuple<std::string, std::string> loginUser(std::string keyfile_name, Database& database) {
    // Open and read the key file
    std::ifstream infile(keyfile_name);
    if (!infile) {
      std::cerr << "Error: Could not open key file '" << keyfile_name << "'" << std::endl;
      return std::make_tuple("", "");
    }
    std::string privateKeyStr((std::istreambuf_iterator<char>(infile)),
                              std::istreambuf_iterator<char>());
    if (infile.fail() && !infile.eof()) {
      std::cerr << "Error: Failed to read data from file '" << keyfile_name << "'" << std::endl;
      infile.close();
      return std::make_tuple("", "");
    }
    infile.close();
    return std::make_tuple(database.getUsernameByPrivateKey(privateKeyStr), privateKeyStr);
  }
}