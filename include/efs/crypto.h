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

#include <cassert>
#define ASSERT assert

using std::unique_ptr;

namespace Efs {
  class Crypto {
    public:
      static std::string generateKeyPair(std::string username);
      static std::string getSha256ForString(std::string inputstring);
      static bool validateKey();
      static void encryptFile(std::string public_key, std::string filepath);
      static void decryptFile(std::string private_key, std::string filepath);
  };
}
