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
  struct KeyPair {
    std::string public_key_filename;
    std::string private_key_filename;
  };
  class Crypto {
    public:
      static KeyPair generateKeyPair(std::string username);
      static std::string getSha256ForString(std::string inputstring);
      static std::string encryptContent(std::string public_key, std::string plaintext);
      static std::string decryptContent(std::string private_key, std::string ciphertext);
      static bool validatePrivateKey(std::string privateKeyStr, std::string publicKeyStr);
      static void encryptFile(std::string public_key, std::string filepath);
      static std::string decryptFile(std::string private_key, std::string filepath);
      static std::string base64Encode(const std::string& input);
      static std::string base64Decode(const std::string& input);
  };
}
