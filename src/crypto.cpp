#include "efs/crypto.h"

std::string Efs::Crypto::generateKeyPair(std::string username) {
  std::string public_key_filename = username + "_public.pem";
  std::string private_key_filename = username + "_private.pem";

  int ret = 0;
  int bits = 2048;
  RSA *rsa = NULL;
  BIGNUM *bne = NULL;
  BIO *bp_public = NULL;
  BIO *bp_private = NULL;

  unsigned long e = RSA_F4;

  try {
    // 1. Generate RSA key
    bne = BN_new();
    ret = BN_set_word(bne, e);
    if (ret != 1) { throw; }

    rsa = RSA_new();
    ret = RSA_generate_key_ex(rsa, bits, bne, NULL);
    if (ret != 1) { throw; }

    // 2. Save the public key
    bp_public = BIO_new_file(public_key_filename.c_str(), "w+");
    ret = PEM_write_bio_RSAPublicKey(bp_public, rsa);
    if (ret != 1) { throw; }

    // 3. Save the private key
    bp_private = BIO_new_file(private_key_filename.c_str(), "w+");
    ret = PEM_write_bio_RSAPrivateKey(bp_private, rsa, NULL, NULL, 0, NULL, NULL);
    if (ret != 1) { throw; }

  } catch (int errorCode) {
    std::cerr << "Unable to generate RSA Key" << std::endl;
  }

  // 4. free
  BIO_free_all(bp_public);
  BIO_free_all(bp_private);
  RSA_free(rsa);
  BN_free(bne);
  return public_key_filename;
}


std::string Efs::Crypto::getSha256ForString(std::string inputstring) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, inputstring.c_str(), inputstring.size());
  SHA256_Final(hash, &sha256);
  std::stringstream ss;
  for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
      ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
  }
  return ss.str();
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
