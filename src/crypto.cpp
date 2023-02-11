#include "efs/crypto.h"

Efs::Crypto::Crypto() {
  // TODO: If needed, use dependency injection
}

std::string Efs::Crypto::generateKey() {
  // TODO: Generate a RSA key (?)
  return "";
}

bool Efs::Crypto::validateKey() {
  // TODO: Validate key here (?)
  return true;
}

void Efs::Crypto::encryptFile(std::string public_key, std::string filepath) {
  // TODO
}

void Efs::Crypto::decryptFile(std::string private_key, std::string filepath) {
  // TODO
}
