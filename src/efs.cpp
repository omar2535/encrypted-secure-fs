#include <iostream>
#include <efs/efs.h>

Efs::Efs::Efs(int argc, char** argv) {
  // if keyfile not inputted, ask for keyfile
  if (argv[1] == NULL) {
    std::cout << "Please enter keyfile!" << std::endl;
    std::cout << "Example: ./fileserver <keyfile_name>" << std::endl;
    return;
  }

  // get the keyfile name
  std::string keyfile_name = argv[1];

  std::cout << keyfile_name << std::endl;
}

Efs::Efs::~Efs() {
}
