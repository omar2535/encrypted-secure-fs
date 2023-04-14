#include <efs/cli.h>

Efs::CLI::CLI(Database* database, std::string username, std::string private_key, std::string initial_dir) {
  this->database = database;
  this->username = username;
  this->private_key = private_key;
  this->v_current_dir = initial_dir;
}

void Efs::CLI::cd(std::string targetDir) {
  // cd / should go to user's root directory
  if (targetDir == "/"){
    this->v_current_dir = "/" + this->username + "/";
    return;
  }

  if (targetDir == "..") {
    if (this->v_current_dir == "/" + this->username + "/"){
      if (username != "admin"){
        std::cout << "Forbidden" << std::endl;
        return;
      }
    }else if (this->v_current_dir == "/")
    {
      return;
    }
    // check if the last character of currentDir is "/"
    bool endsWithSlash = this->v_current_dir.back() == '/';

    // find the last '/' before the trailing '/'
    size_t lastSlashPos = this->v_current_dir.find_last_of("/", this->v_current_dir.length() - (endsWithSlash ? 2 : 1));

    this->v_current_dir = this->v_current_dir.substr(0, lastSlashPos + 1);
    return;
  } else if (targetDir == ".") {
    return;
  } else {
    // make sure targetDir is a directory
    if (targetDir.back() != '/') {
      targetDir += "/";
    }

    // Split the targetDir string into individual directory names
    std::vector<std::string> dirs = Utils::splitString(targetDir, '/');

    // Construct a temporary directory path
    std::string temp_dir_path = this->v_current_dir;
    for (const auto& dir : dirs) {
      if (dir == "..") {
        bool endsWithSlash = temp_dir_path.back() == '/';
        size_t lastSlashPos = temp_dir_path.find_last_of("/", temp_dir_path.length() - (endsWithSlash ? 2 : 1));
        temp_dir_path = temp_dir_path.substr(0, lastSlashPos + 1);
      } else {
        temp_dir_path += dir + "/";
      }
    }

    // if the user isn't admin
    if (this->username != "admin") {
      // if not the owner of the path
      if (Utils::getOwnerOfPath(temp_dir_path) != this->username) {
        std::cout << "Forbidden" << std::endl;
        return;
      }

      // if directory doesn't exist
      if (!database->doesDirExist(temp_dir_path)) {
        std::cout << "Target directory does not exist" << std::endl;
        return;
      }

      this->v_current_dir = temp_dir_path;
      return;
    }

    // if user is admin
    if (this->username == "admin") {
      // if directory doesn't exist
      if (!database->doesDirExist(temp_dir_path) && temp_dir_path != "/") {
        std::cout << "Target directory doesn't exist" << std::endl;
        return;
      }
      this->v_current_dir = temp_dir_path;
      return;
    }
  }
}

void Efs::CLI::pwd() {
  std::cout << v_current_dir << std::endl;
}

void Efs::CLI::ls() {
  //create a temporary entries vector read json path from database
  std::vector<std::string> t_entries;
  t_entries = this->database->getAllFilePaths();

  //create a entries vector store the actual ls entries
  std::vector<std::string> entries;

  for (const auto& t_entry : t_entries) {
  	std::string path = t_entry;

  	if (path.find(this->v_current_dir) == 0) {
      // the path starts with current_dir, so remove the prefix
      path.erase(0, this->v_current_dir.length());
      //remove subpath or filename under this directory layer
      if(path.find('/')<path.length()&& path !=""){
        path=path.substr(0, path.find("/")+1);
      }
      //check if the reduced path is empty string
      if(path!=""){
        //store modified path into entries
        entries.push_back(path);
      }
  	}
  }

  //sort entries alphabetically
  std::sort(entries.begin(), entries.end());
  //remove duplicate entry name
  entries.erase(std::unique(entries.begin(), entries.end()), entries.end());

  //print . and ..
  std::cout << "d -> . " << std::endl;
  std::cout << "d -> .." << std::endl;

  //print out entries vector
   for (const auto& entry : entries) {
   	//determine if the entry is a file or directory
   	char fileType;
    	if (entry.back()=='/'){
        	fileType='d';
        }
        else{
        	fileType='f';
        }
  	std::cout << fileType <<" -> "<< entry << std::endl;
  }
}


void Efs::CLI::cat(std::string filename) {
  // construct the filepath
  std::string filepath = this->v_current_dir + filename;

  // Check if the file exists
  if (!this->database->doesFileExist(filepath)) {
    std::cout << "File does not exist: " << filepath << std::endl;
    return;
  }

  // actual read:
  // if admin, user private key of other users
  // otherwise use my own private key
  try {
    if (this->username == "admin") {
      std::string impersonated_user = Utils::getOwnerOfPath(filepath);
      std::string impersonated_private_key = this->database->getPrivateKeyForUser(impersonated_user);
      std::cout << this->filesystem_service.readFile(filepath, impersonated_private_key) << std::endl;
    } else {
      std::cout << this->filesystem_service.readFile(filepath, this->private_key) << std::endl;;
    }
  } catch (FilesystemService::ReadFileException &ex) {
    std::cout << "Unable to read file" << std::endl;
    return;
  }
}

void Efs::CLI::share(std::string filename, std::string target_user) {
  if (!this->database->doesUserExist(target_user)) {
    std::cout << "User does not exist" << std::endl;
    return;
  }

  // set some initial variables
  std::string src_filepath = this->v_current_dir + filename;
  std::string dst_dirpath = "/" + target_user + "/shared/" + this->username + "/";
  std::string src_private_key = this->private_key;
  std::string dst_public_key = this->database->getPublicKeyForUser(target_user);

  // Consistency checking first
  if (filename.back() == '/') {
    std::cout << "Cannot share a directory" << std::endl;
    return;
  }
  if (this->username == target_user) {
    std::cout << "Target user cannot be current user" << std::endl;
    return;
  }
  if (!this->database->doesFileExist(src_filepath)) {
    std::cout << "File does not exist" << std::endl;
    return;
  }


  // ADD SOURCE USER'S SHARE FOLDER IF NOT IN THE TARGET'S SHARED
  if (!database->doesDirExist(dst_dirpath)) {
    bool res = this->filesystem_service.createDirectory(dst_dirpath);
    this->database->addDir(dst_dirpath);
  }

  /* MAIN PART OF THE PROGRAM */
  // 1. Construct the destination path
  std::string dst_filepath = dst_dirpath + filename;

  // 2. Check if the destination filepath already exists
  if (database->doesFileExist(dst_filepath)) {
    std::cout << "A file with the same name was already shared. Please rename the file to share" << std::endl;
    // TODO: Ask if the user would like to replace the file (in which case just run copy file and return)
    return;
  }

  // 3. Copy the file over
  this->filesystem_service.copyFile(src_filepath, dst_filepath, src_private_key, dst_public_key);

  // 4. Add file entry to target user
  this->database->addFile(dst_filepath);

  // 5. Add shared entry to shared
  this->database->addSharedFileForFile(src_filepath, dst_filepath);

  // 6. Print out that file has been shared
  std::cout << "Shared file with " + target_user + " at " + dst_filepath << std::endl;
}


void Efs::CLI::mkdir(std::string dirname) {
  if (!Utils::isInPersonal(this->v_current_dir, this->username)){
    std::cout << "Forbidden" << std::endl;
    return;
  }

  // case: If dirname is . or ..
  if(!Utils::isValidName(dirname)) {
  	std::cout << "Invalid directory name" << std::endl;
    return;
  }

  // make sure dirname has slash at the end
  if (dirname.back() != '/') dirname += "/";

  // set the entire dirpath
  std::string dirpath = this->v_current_dir + dirname;

  // case: If a file with the same name exists
  std::string filepath = dirpath.substr(0, dirpath.size()-1);
  if (this->database->doesFileExist(filepath)) {
    std::cout << "File with same name already exists. Abandoning." << std::endl;
    return;
  }

  // case: if directory already exists
  if (this->database->doesDirExist(dirpath)) {
    std::cout << "Directory already exists" << std::endl;
    return;
  }

  try {
    // 1. Register directory to database and encrypt the directory path
    //    with the current user's public key
    this->database->addDir(dirpath);

    // 2. On operating system, create the directory in the current directory
    //    with the hashed directory name
    this->filesystem_service.createDirectory(dirpath);

    // 3. Print out success
    std::cout << "Created directory: " << dirpath << std::endl;
  } catch (FilesystemService::CreateDirectoryException &ex) {
    std::cout << "Error creating directory" << std::endl;
  }
}

// makes the file with the given contents
// if already exists, overwrite the file and re-share with all shared users
void Efs::CLI::mkfile(std::string filename, std::string contents) {
  if (!Utils::isInPersonal(this->v_current_dir, this->username)){
    std::cout << "Forbidden" << std::endl;
    return;
  }

  // set some initial variables
  std::string filepath = this->v_current_dir + filename;
  std::string public_key = this->database->getPublicKeyForUser(this->username);

  // case: check if filename is valid
  if (!Utils::isValidName(filename)) {
    std::cout << "Invalid filename" << std::endl;
    return;
  }

  // case: If a directory with the same name exits
  if (this->database->doesDirExist(filepath + '/')) {
    std::cout << "A directory with the same name already exists. Not creating." << std::endl;
    return;
  }

  // IF FILE ALREADY EXISTS, RE-CREATE IT AN SHARE WITH OTHER USERS
  if (this->database->doesFileExist(filepath)) {
    std::cout << "File already exists, overriting" << std::endl;
    // create the file (overrite)
    this->filesystem_service.createFile(filepath, contents, public_key);

    // reshare with all users
    std::vector<std::string> dst_filepaths = this->database->getSharedFilesForFile(filepath);
    for (std::string dst_filepath : dst_filepaths) {
      std::string dst_username = Utils::getOwnerOfPath(dst_filepath);
      std::string dst_public_key = this->database->getPublicKeyForUser(dst_username);
      this->filesystem_service.copyFile(filepath, dst_filepath, this->private_key, dst_public_key);
    }
  }

  // DEFAULT OPERATION: creating a file when it didn't exist previously
  try {
    // 1. Register file to database
    this->database->addFile(filepath);

    // 2. Create the file
    this->filesystem_service.createFile(filepath, contents, public_key);
    std::cout << "Created file!" << std::endl;
  } catch (FilesystemService::CreateFileException &ex) {
    std::cout << "Internal error: Unable to create file" << std::endl;
  }
}

// Add a new user
void Efs::CLI::adduser(std::string username) {
  UserManager userManager(this->database);

  // check if the user is admin
  if (this->username != "admin") {
    std::cout << "Forbidden" << std::endl;
    return;
  }

  // Check if the username already exists in the database
  if (this->database->doesUserExist(username)) {
    std::cout << "User " << username << " already exists" << std::endl;
    return;
  }
  // Check if username contains any illegal characters such as ., .., or /
  const std::string symbols = "~`!@#$%^&*()-_+={}[]|\\:;\"'<>,.?/";
  if(username.find_first_of(symbols) != std::string::npos){
  	std::cout << "Username cannot contain any of the following symbols: " << symbols << std::endl;
  	return;
  }

  // Create the user in the database
  userManager.createUser(username);

  // Print out success
  std::cout << "Created user: " << username << std::endl;
}
