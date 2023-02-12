#include "efs/admin.h"

void Efs::Admin::initilizeAdmin() {
  Crypto::generateKeyPair(this->username);
  // TODO: Initialize the admin
}



/* PRIVATE METHODS */
void Efs::Admin::initializeAdminFilesystem() {
  // TODO: Initialize admin filesystem here
}
