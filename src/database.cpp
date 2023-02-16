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
void Efs::Database::addFile(std::string filepath) {
  std::string hash_filepath = Crypto::getSha256ForString(filepath);
  this->file_mappings_json[hash_filepath] = filepath;
  this->saveDbState();
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


/* ------------------------------------- */

/* ------------------------------------- */





bool Efs::Database::isDatabaseInitialized() {
  // the DB consists of two JSON files.
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
