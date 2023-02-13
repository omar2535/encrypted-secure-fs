#include "efs/admin.h"

void Efs::Admin::initilizeAdmin() {
  this->initializeAdminKeyPair();
  std::cout << "Initilized admin keypair" << std::endl;
  this->initializeAdminFilesystem();

  // TODO: Initialize the admin filesystem
  // TODO: Initialize the admin entry in shared
  // TODO: Initialize the admmin in DB
  // TODO: Initialize the admin
}



/* PRIVATE METHODS */
void Efs::Admin::initializeAdminFilesystem() {
  // TODO: Initialize admin filesystem here
  Filesystem filesystem("admin", "db/admin_db.json", "asdf");
}

void Efs::Admin::initializeAdminKeyPair() {
  Crypto::generateKeyPair(this->username);
}
