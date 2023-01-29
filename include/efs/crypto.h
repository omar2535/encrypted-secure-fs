#pragma once

#include <openssl/bio.h> /* BasicInput/Output streams */
#include <openssl/err.h> /* errors */
#include <openssl/ssl.h> /* core library */
#include <iostream>

namespace Efs {
  class Crypto {
    public:
      /**
       * @brief Construct a new Crypto object
       * Currently shouldn't do anything
       */
      Crypto();
      std::string generateKey();
      bool validateKey();
  };
}
