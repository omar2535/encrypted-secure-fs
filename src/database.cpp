#include "efs/database.h"

Efs::Database::Database() {
  // blank constructor
}

Efs::Database::~Database() {
  // blank destructor
}

bool Efs::Database::isDatabaseInitialized() {
  // the DB consists of two JSON files.
  return (
    std::filesystem::exists(this->SHARED_FILE) &&
    std::filesystem::exists(this->USER_INFO_FILE)
  );
}

void Efs::Database::initializeDatabase() {
  // the DB consists of two JSON files.
  if (!std::filesystem::exists(this->SHARED_FILE)) {
    std::ofstream output(this->SHARED_FILE);
  }

  if (!std::filesystem::exists(this->USER_INFO_FILE)) {
    std::ofstream output(this->USER_INFO_FILE);
  }
}
