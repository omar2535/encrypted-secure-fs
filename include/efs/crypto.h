#pragma once

#include <openssl/bio.h> /* BasicInput/Output streams */
#include <openssl/err.h> /* errors */
#include <openssl/ssl.h> /* core library */
#include <iostream>

namespace Efs {
  class Crypto {
    public:
      Crypto();
      std::string generateKey();
      bool validateKey();
      void encryptFile(std::string public_key, std::string filepath);
      void decryptFile(std::string private_key, std::string filepath);
  };
}
