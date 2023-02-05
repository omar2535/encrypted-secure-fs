#include "efs/filesystem.h"


Efs::Filesystem::Filesystem() {
  // create DB file if it doesn't exist yet
  if (!std::filesystem::exists(this->DB_FILE)) {
    std::ofstream output(this->DB_FILE);
  }

  if (!std::filesystem::exists(this->SHARED_FILE)) {
    std::ofstream output(this->SHARED_FILE);
  }

  if (!std::filesystem::exists(this->KEYS_FILE)) {
    std::ofstream output(this->KEYS_FILE);
  }
}




Efs::Filesystem::~Filesystem() {
  // do nothing destructor
}
