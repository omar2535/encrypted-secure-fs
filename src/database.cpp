#include "efs/database.h"

// Constructor: Creates db files if doesn't exist
// otherwise loads file data into json instance variables
Efs::Database::Database() {
  if (!this->isDatabaseInitialized()) {
    this->initializeDatabase();
  }

  std::ifstream fmf(this->FILE_MAPPINGS_FILE);
  std::ifstream sf(this->SHARED_FILE);
  std::ifstream uif(this->USER_INFO_FILE);
  std::ifstream pkf(this->PRIVATE_KEY_FILE);

  this->file_mappings_json = nlohmann::json::parse(fmf);
  this->shared_json = nlohmann::json::parse(sf);
  this->user_info_json = nlohmann::json::parse(uif);
}

// Destructor: Save the DB before dying
Efs::Database::~Database() {
  this->saveDbState();
}


/* ------------------------------------- */
/* FOR FILE_MAPPINGS_FILE */
//get entire file path from file_mappings.json
std::vector<std::string> Efs::Database::getAllFilePaths() {
  std::vector<std::string> file_paths;
  for (auto& element : this->file_mappings_json.items())
    file_paths.push_back(element.value());
  return file_paths;
}

// Add a file to the file_mappings.json by getting its SHA256 sum
std::string Efs::Database::addFile(std::string filepath) {
  std::string hash_filepath = Crypto::getSha256ForString(filepath);
  this->file_mappings_json[hash_filepath] = filepath;
  this->saveDbState();

  return hash_filepath;
}

std::string Efs::Database::addDir(std::string dirpath) {
  std::string hash_Dirpath = Crypto::getSha256ForString(dirpath);
  this->file_mappings_json[hash_Dirpath] = dirpath;
  this->saveDbState();

  return hash_Dirpath;
}

// delete file from database
void Efs::Database::deleteFile(std::string filepath) {
  std::string hash_filepath = Crypto::getSha256ForString(filepath);
  this->file_mappings_json.erase(hash_filepath);
  this->saveDbState();
}

// get filepath from hash
std::string Efs::Database::getFilepathFromSha256(std::string hash_string) {
  // first, check if it is inside the file_mappings_json
  if (!this->file_mappings_json.contains(hash_string)) return "";

  try {
    std::string filepath = this->file_mappings_json[hash_string].get<std::string>();
    return filepath;
  } catch (nlohmann::json_abi_v3_11_2::detail::type_error error) {
    // Silently error in case there's some issue
    return "";
  }
}

// get hash from filepath
std::string Efs::Database::getSha256FromFilePath(std::string filepath) {
  if (std::filesystem::is_directory(filepath)) {
    filepath += "/";
  }
  return Efs::Crypto::getSha256ForString(filepath);
}

// checks if file exists
// true if it exists, false otherwise
bool Efs::Database::doesFileExist(std::string v_filepath) {
  std::string file_hash = Crypto::getSha256ForString(v_filepath);
  return this->getFilepathFromSha256(file_hash) != "";
}

bool Efs::Database::doesDirExist(std::string v_dirpath) {
  std::string dir_hash = Crypto::getSha256ForString(v_dirpath);
  return this->getFilepathFromSha256(dir_hash) != "";
}


/* ------------------------------------- */

/* ------------------------------------- */
/* For Users_info.json */

// create a new user
// assumes user doesn't already exist
// assumes only admin can call this function
void Efs::Database::createUser(std::string username) {
  if (username == "admin") this->is_admin = true;

  this->user_info_json[username] = {};
  this->private_keys_json[username] = {};
  // create their public key and private key
  KeyPair key_pair = Crypto::generateKeyPair(username);

  /* For Users_info.json */

  // save the public key into the users_info.json
  std::ifstream ifs(key_pair.public_key_filename);
  std::string public_key_contents((std::istreambuf_iterator<char>(ifs)),
                                  (std::istreambuf_iterator<char>()));

  // save the public key under the user
  this->user_info_json[username]["public_key"] = public_key_contents;

  // initialize user's file mappings too
  std::string user_root_dir_hash = this->addFile("/" + username + "/");
  std::string user_personal_dir_hash = this->addFile("/" + username + "/personal/");
  std::string user_share_dir_hash = this->addFile("/" + username + "/shared/");

  // save each of the directories under the user as well
  this->user_info_json[username]["root_dir_hash"] = user_root_dir_hash;
  this->user_info_json[username]["personal_dir_hash"] = user_personal_dir_hash;
  this->user_info_json[username]["share_dir_hash"] = user_share_dir_hash;

  /* For Private_keys.json */
  // save the private key into the private_keys.json
  std::ifstream ifs2(key_pair.private_key_filename);
  std::string private_key_contents((std::istreambuf_iterator<char>(ifs2)),
                                   (std::istreambuf_iterator<char>()));

  // save the private key under the user
  this->private_keys_json[username] = private_key_contents;

  // save the json contents into the file
  this->saveDbState();
}

// check if the user exists
bool Efs::Database::doesUserExist(std::string username) {
  return this->user_info_json.contains(username);
}

// get the key for the user
std::string Efs::Database::getPublicKeyForUser(std::string username) {
  return this->getPropertyForUserInfo(username, "public_key");
}

// get username by private key
std::string Efs::Database::getUsernameByPrivateKey(std::string private_key) {
  for (auto& [username, obj] : this->user_info_json.items()) {
    if (Crypto::validatePrivateKey(private_key, obj["public_key"])) {
      return username;
    }
  }
  return "";
}

/* ------------------------------------- */

/* ------------------------------------- */
/* For Shared.json */
void Efs::Database::addSharedFileForFile(std::string src_filepath, std::string dst_filepath) {
  std::string src_filepath_hash = Crypto::getSha256ForString(src_filepath);
  std::string dst_filepath_hash = Crypto::getSha256ForString(dst_filepath);

  if (!this->shared_json.contains(src_filepath_hash)) {
    this->shared_json[src_filepath_hash] = {dst_filepath_hash};
  } else {
    this->shared_json[src_filepath_hash].push_back(dst_filepath_hash);
  }
}

std::vector<std::string> Efs::Database::getSharedFilesForFile(std::string src_filepath) {
  std::string src_filepath_hash = Crypto::getSha256ForString(src_filepath);
  std::vector<std::string> dst_filepaths = {};

  if (this->shared_json.contains(src_filepath_hash)) {
    std::vector<std::string> res = this->shared_json[src_filepath_hash].get<std::vector<std::string>>();
    for (std::string dst_filepath_hash : res) {
      std::string dst_filepath = this->getFilepathFromSha256(dst_filepath_hash);
      dst_filepaths.push_back(dst_filepath);
    }
  }
  return dst_filepaths;
}

// Checks if the database is initialized
// the DB consists of three JSON files.
bool Efs::Database::isDatabaseInitialized() {
  return (
    std::filesystem::exists(this->FILE_MAPPINGS_FILE) &&
    std::filesystem::exists(this->SHARED_FILE) &&
    std::filesystem::exists(this->USER_INFO_FILE)
  );
}

// creates empty files for the db files not initialized yet
void Efs::Database::initializeDatabase() {
  for (auto const& [dbfile, dbjson] : this->db_files_map) {
    std::ofstream output(dbfile);
    output << nlohmann::json::object();
    output.close();
  }
}

void Efs::Database::initializeAdminDatabase(std::string private_key) {
  this->is_admin = true;

  if (!std::filesystem::exists(this->PRIVATE_KEY_FILE)) {
    std::ofstream output(this->PRIVATE_KEY_FILE);
    output << nlohmann::json::object();
    output.close();
    this->private_keys_json = nlohmann::json::object();
  } else {
    std::string private_key_file_decrypted = Crypto::decryptFile(private_key, this->PRIVATE_KEY_FILE);
    // std::ifstream private_key_file_decrypted(this->PRIVATE_KEY_FILE);
    this->private_keys_json = nlohmann::json::parse(private_key_file_decrypted);
  }
}

// Get's a user's property based on the key
// assuems the retrieved property is a string
std::string Efs::Database::getPropertyForUserInfo(std::string username, std::string property) {
  try {
    return this->user_info_json[username][property].get<std::string>();
  } catch (nlohmann::json_abi_v3_11_2::detail::type_error error) {
    // means the username doesn't exist, just return an empty string
    return "";
  }
}


/* ------------------------------------- */
/* For Private_keys.json */
std::string Efs::Database::getPrivateKeyForUser(std::string username) {
  try {
    return this->private_keys_json[username].get<std::string>();
  } catch (const std::exception &ex) {
    return "";
  }
}


/* PRIVATE METHODS */
void Efs::Database::saveDbState() {
  for (auto const& [dbfile, dbjson] : this->db_files_map) {
    std::ofstream output(dbfile);
    output << std::setw(2) << *dbjson << std::endl;
    output.close();
  }

  if (this->is_admin) {
    this->savePrivateKeysState();
  }
}

// save private keys file and then encrypt
void Efs::Database::savePrivateKeysState() {
  std::ofstream output(this->PRIVATE_KEY_FILE);
  output << std::setw(2) << this->private_keys_json << std::endl;
  output.close();


  try {
    std::string admin_public_key = this->getPublicKeyForUser("admin");
    std::string file_path = this->PRIVATE_KEY_FILE;
    Crypto::encryptFile(admin_public_key, file_path);
  } catch (const std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }
}
