#include "efs/crypto.h"

//Change this to return a struct with public and private key
Efs::KeyPair Efs::Crypto::generateKeyPair(std::string username) {

  int ret = 0;
  int bits = 2048;
  RSA *rsa = NULL;
  BIGNUM *bne = NULL;
  BIO *bp_public = NULL;
  BIO *bp_private = NULL;
  KeyPair keyPair;

  keyPair.public_key_filename = username + "_public.pem";
  keyPair.private_key_filename = username + "_private.pem";

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
    bp_public = BIO_new_file(keyPair.public_key_filename.c_str(), "w+");
    ret = PEM_write_bio_RSAPublicKey(bp_public, rsa);
    if (ret != 1) { throw; }

    // 3. Save the private key
    bp_private = BIO_new_file(keyPair.private_key_filename.c_str(), "w+");
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
  return keyPair;
}


std::string Efs::Crypto::getSha256ForString(std::string inputstring) {
  std::string SALT_VAL = "asupersecretsalt";

  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, inputstring.c_str(), inputstring.size());
  SHA256_Update(&sha256, SALT_VAL.c_str(), SALT_VAL.size());
  SHA256_Final(hash, &sha256);
  std::stringstream ss;
  for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
      ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
  }
  return ss.str();
}

/* ENCRYPTS A STRING GIVEN A PUBLIC KEY. ENCRYPTS ONLY UP TO THE PUBLIC_KEY_SIZE. ANY MORE WILL CAUSE ERROR*/
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
  const unsigned char* plaintext_char = reinterpret_cast<const unsigned char*>(plaintext.c_str());
  unsigned char* ciphertext_char  = new unsigned char[RSA_size(pub_rsa)];
  int ciphertext_length = RSA_public_encrypt(plaintext.size(), plaintext_char, ciphertext_char, pub_rsa, RSA_PKCS1_OAEP_PADDING);
  if ( -1 == ciphertext_length) {
    RSA_free(pub_rsa);
    delete[] ciphertext_char;
    throw std::runtime_error("Encryption failed");
  } else {
    return std::string (reinterpret_cast<char*>(ciphertext_char), ciphertext_length);
  }
}

std::string Efs::Crypto::decryptContent(std::string private_key, std::string ciphertext) {
  if (ciphertext.length() == 0) {
    throw std::invalid_argument("Empty ciphertext");
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
    // suppress when login
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
  // chunk the ciphertext
  std::string ciphertext = "";
  int CHUNK_SIZE = 100;

  for (int i = 0; i < plaintext.length(); i+= CHUNK_SIZE) {
    int remaining_bytes = plaintext.length() - i;
    std::string plaintext_chunk;
    if (remaining_bytes < CHUNK_SIZE) {
      plaintext_chunk = plaintext.substr(i, remaining_bytes);
    } else {
      plaintext_chunk = plaintext.substr(i, CHUNK_SIZE);
    }
    ciphertext += encryptContent(public_key, plaintext_chunk);
  }

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

  std::string plaintext = "";
  int CHUNK_SIZE = 256;
  for (int i=0; i < ciphertext.length(); i+= CHUNK_SIZE) {
    int remaining_bytes = ciphertext.length() - i;
    std::string ciphertext_chunk;
    if (remaining_bytes < CHUNK_SIZE) {
      ciphertext_chunk = ciphertext.substr(i, remaining_bytes);
    } else {
      ciphertext_chunk = ciphertext.substr(i, CHUNK_SIZE);
    }
    plaintext += decryptContent(private_key, ciphertext_chunk);
  }
  return plaintext;
}
