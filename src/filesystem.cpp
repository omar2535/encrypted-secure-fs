#include "efs/filesystem.h"


Efs::Filesystem::Filesystem(std::string username, std::string filepath, std::string private_key_filepath) {
  this->username = username;
  this->filepath = filepath;
  this->private_key_filepath = private_key_filepath;

  // TODO: Use the private key to decrypt
  if (!std::filesystem::exists(filepath)) {
    this->initializeNewFilesystem();
  }
}



Efs::Filesystem::~Filesystem() {
  // do nothing destructor
}


void Efs::Filesystem::initializeNewFilesystem() {
  std::ofstream output(filepath);
  std::string root_dir = "/" + this->username + "/";
  std::string personal_dir = root_dir + "personal/";
  std::string shared_dir = root_dir + "shared/";

  // create the intiial structure for user
  this->json = {
    {root_dir, {
      {"type", "directory"},
      {"children", {
        {personal_dir, {
          {"type", "directory"},
          {"children", {}}
        }},
        {shared_dir, {
          {"type", "directory"},
          {"children", {}}
        }}
      }}
    }}
  };
  output << this->json;
}
