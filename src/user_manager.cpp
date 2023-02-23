#include <efs/user_manager.h>

// Simple constructor
Efs::UserManager::UserManager(Database* database) {
  this->database = database;
}

// Create user if user doesn't exist
// initializes user's directories as well
// returns the user's home directory (IE. /admin/)
std::string Efs::UserManager::createUser(std::string username) {
  std::string user_home_dir = "/" + username + "/";

  if (!this->database->doesUserExist(username)) {
    this->database->createUser(username);
    FilesystemUtils::createDir(user_home_dir);
    FilesystemUtils::createDir(user_home_dir + "/personal/");
    FilesystemUtils::createDir(user_home_dir+ "/shared/");
  }

  return user_home_dir;
}
