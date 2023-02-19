#pragma once

#include <memory>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/bio.h> /* BasicInput/Output streams */
#include <openssl/err.h> /* errors */
#include <openssl/ssl.h> /* core library */
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <cassert>
#define ASSERT assert

using std::unique_ptr;

namespace Efs {
  class Crypto {
    public:
      static std::string generateKeyPair(std::string username);
      static std::string getSha256ForString(std::string inputstring);
      static std::string encryptContent(std::string public_key, std::string plaintext);
      static std::string decryptContent(std::string private_key, std::string ciphertext);
      static bool validatePrivateKey(std::string privateKeyStr, std::string publicKeyStr);
      static void encryptFile(std::string public_key, std::string filepath);
      static std::string decryptFile(std::string private_key, std::string filepath);
  };
}
