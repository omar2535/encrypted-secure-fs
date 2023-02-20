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
  try {
    std::string filepath = this->file_mappings_json[hash_string].get<std::string>();
    return filepath;
  } catch (nlohmann::json_abi_v3_11_2::detail::type_error error) {
    // means the hash string doesn't exist, just return an empty string
    return "";
  }
}

// checks if file exists
// true if it exists, false otherwise
bool Efs::Database::doesFileExist(std::string filepath) {
  std::string file_hash = Crypto::getSha256ForString(filepath);
  return this->getFilepathFromSha256(file_hash) != "";
}

bool Efs::Database::doesDirExist(std::string dirpath) {
  std::string Dir_hash = Crypto::getSha256ForString(dirpath);
  return this->getFilepathFromSha256(dirpath) != "";
}


/* ------------------------------------- */

/* ------------------------------------- */
/* For Users_info.json */

// create a new user
// assumes user doesn't already exist
void Efs::Database::createUser(std::string username) {
  this->user_info_json[username] = {};
  
  // create their public key and private key
  std::string public_key_filename = Crypto::generateKeyPair(username);
  
  // save the public key into the users_info.json
  std::ifstream ifs(public_key_filename);
  std::string public_key_contents((std::istreambuf_iterator<char>(ifs)),
                                  (std::istreambuf_iterator<char>()));
  
  // save the public key under the user
  this->user_info_json[username]["public_key"] = public_key_contents;

  // initialize user's file mappings too
  std::string user_root_dir_hash = this->addFile(username + "/");
  std::string user_personal_dir_hash = this->addFile(username + "/personal/");
  std::string user_share_dir_hash = this->addFile(username + "/shared/");

  // save each of the directories under the user as well
  this->user_info_json[username]["root_dir_hash"] = user_root_dir_hash;
  this->user_info_json[username]["personal_dir_hash"] = user_personal_dir_hash;
  this->user_info_json[username]["share_dir_hash"] = user_share_dir_hash;

  // save the json contents into the file
  this->saveDbState();
}

// check if the user exists
bool Efs::Database::doesUserExist(std::string username) {
  return this->user_info_json.contains(username);
}

// get the key for the user
std::string Efs::Database::getPublicKeyForUser(std::string username) {
  try {
    return this->user_info_json[username]["public_key"].get<std::string>();
  } catch (nlohmann::json_abi_v3_11_2::detail::type_error error) {
    // means the username doesn't exist, just return an empty string
    return "";
  }
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


/* PRIVATE METHODS */
void Efs::Database::saveDbState() {
  for (auto const& [dbfile, dbjson] : this->db_files_map) {
    std::ofstream output(dbfile);
    output << std::setw(2) << *dbjson << std::endl;
    output.close();
  }
}
