#include "efs/admin.h"

void Efs::Admin::initilizeAdmin() {
  this->initializeAdminKeyPair();
}



/* PRIVATE METHODS */
void Efs::Admin::initializeAdminFilesystem() {
  // TODO: Initialize admin filesystem here
  Filesystem filesystem("admin", "db/admin_db.json", "asdf");
}

void Efs::Admin::initializeAdminKeyPair() {
  Crypto::generateKeyPair(this->username);
}
