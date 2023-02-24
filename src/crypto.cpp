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


std::string Efs::Crypto::encryptContent(std::string public_key, std::string plaintext) {
  if (plaintext.length() == 0) {
    throw std::invalid_argument("Empty plaintext");
  }
  // Get public key
  BIO *pub_bio = BIO_new_mem_buf(public_key.c_str(), -1);
  if (!pub_bio) {
    BIO_free(pub_bio);
    throw std::invalid_argument("Invalid pubilc key");
  }

  RSA *pub_rsa = PEM_read_bio_RSAPublicKey(pub_bio, NULL, NULL, NULL);
  if (!pub_rsa) {
    throw std::invalid_argument("Invalid pubilc key");
  }
  BIO_free(pub_bio);

  // Encrypt
  unsigned char* plaintext_char = reinterpret_cast<unsigned char*>(&plaintext[0]);
  unsigned char* ciphertext_char  = new unsigned char[RSA_size(pub_rsa)];
  int ciphertext_length = RSA_public_encrypt(plaintext.length(), plaintext_char, ciphertext_char, pub_rsa, RSA_PKCS1_OAEP_PADDING);
  if ( -1 == ciphertext_length) {
    throw std::runtime_error("Encryption failed");
  } else {
    return std::string (reinterpret_cast<char*>(ciphertext_char), ciphertext_length);
  }
}


std::string Efs::Crypto::decryptContent(std::string private_key, std::string ciphertext) {
  if (ciphertext.length() == 0) {
    throw std::invalid_argument("Empty plaintext");
  }
  // Get privat key
  BIO *pri_bio = BIO_new_mem_buf(private_key.c_str(), -1);
  if (!pri_bio) {
    BIO_free(pri_bio);
    throw std::invalid_argument("Invalid private key");
  }
  RSA *pri_rsa = PEM_read_bio_RSAPrivateKey(pri_bio, NULL, NULL, NULL);
  if (!pri_rsa) {
    throw std::invalid_argument("Invalid private key");
  }
  BIO_free(pri_bio);

  // Decrypt
  unsigned char* ciphertext_char = reinterpret_cast<unsigned char*>(&ciphertext[0]);
  unsigned char* plaintext_char  = new unsigned char[2048];
  int plaintext_length = RSA_private_decrypt(RSA_size(pri_rsa), ciphertext_char, plaintext_char, pri_rsa, RSA_PKCS1_OAEP_PADDING);
  if (-1 == plaintext_length) {
    throw std::runtime_error("Decryption failed");
  } else {
    return std::string (reinterpret_cast<char*>(plaintext_char), plaintext_length);
  }
}


bool Efs::Crypto::validatePrivateKey(std::string privateKeyStr, std::string publicKeyStr) {
  // Check if the private key and public key match
  const std::string secret = "secret";
  std::string res;
  try {
    res = decryptContent(privateKeyStr, encryptContent(publicKeyStr, secret));
  }
  catch(const std::exception& e) {
    // surpress when login
    // std::cerr << e.what() << '\n';
  }
  return res == secret;
}


void Efs::Crypto::encryptFile(std::string public_key, std::string filepath) {
  // read file
  std::ifstream infile(filepath);
  if (!infile) {
    std::cerr << "Error: Could not open key file '" << filepath << "'" << std::endl;
    return;
  }
  std::string plaintext((std::istreambuf_iterator<char>(infile)),
                            std::istreambuf_iterator<char>());
  if (infile.fail() && !infile.eof()) {
    std::cerr << "Error: Failed to read data from file '" << filepath << "'" << std::endl;
    infile.close();
    return;
  }
  infile.close();

  // encrypt
  std::string ciphertext = encryptContent(public_key, plaintext);

  // write file
  std::ofstream outfile(filepath, std::ios::trunc);
  if (!outfile) {
    std::cerr << "Error: Could not open file '" << filepath << "'" << std::endl;
    return;
  }
  outfile << ciphertext;
  if (outfile.fail()) {
    std::cerr << "Error: Failed to write data to file '" << ciphertext << "'" << std::endl;
    outfile.close();
    return;
  }
  outfile.close();
  if (outfile.fail()) {
    std::cerr << "Error: Failed to close file '" << ciphertext << "'" << std::endl;
    return;
  }
}

std::string Efs::Crypto::decryptFile(std::string private_key, std::string filepath) {
  // read file
  std::ifstream infile(filepath);
  if (!infile) {
    std::cerr << "Error: Could not open key file '" << filepath << "'" << std::endl;
    throw std::runtime_error("Invalid filepath");
  }
  std::string ciphertext((std::istreambuf_iterator<char>(infile)),
                          std::istreambuf_iterator<char>());
  if (infile.fail() && !infile.eof()) {
    std::cerr << "Error: Failed to read data from file '" << filepath << "'" << std::endl;
    infile.close();
    throw std::runtime_error("Error reading file");
  }
  infile.close();
  return decryptContent(private_key, ciphertext);
}
