#include "efs/user.h"

Efs::User::User(std::string username) {
  this->username = username;
}

std::string Efs::User::getUsername() {
  return this->username;
}
