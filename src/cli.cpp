#include <efs/cli.h>

Efs::CLI::CLI(Database* database, std::string username, std::string private_key, std::string initial_dir) {
  this->database = database;
  this->username = username;
  this->private_key = private_key;
  this->v_current_dir = initial_dir;
}

void Efs::CLI::cd(std::string targetDir) {
  if (targetDir == "..") {
    if (this->v_current_dir == "/" + username + "/"){
      if (username != "admin"){
        std::cout << "Permission denied" << std::endl;
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
    std::string newDir = this->v_current_dir + targetDir;
    if (newDir.find_last_of("/")!= newDir.length() - 1) {
      newDir += "/";
    }

    std::ifstream file("File_mappings.json");
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileContents = buffer.str();
    if (fileContents.find(newDir) != std::string::npos) {
      this->v_current_dir = newDir;
    } else {
      std::cout << "Target directory does not exist" << std::endl;
      return;
    }
  }
}

void Efs::CLI::pwd() {
  std::cout << v_current_dir << std::endl;
}

void Efs::CLI::ls() {
  //create entries to store each json path string
  std::vector<std::string> entries;

  //Open file mappings json file
  std::ifstream f("File_mappings.json");
  json data = json::parse(f);

  for (auto& element : data.items()) {
  	//read entire path in json, /admin/personal/personalsub/
  	std::string path = element.value();

  	if (path.find(this->v_current_dir) == 0) {
      // the path starts with current_dir, so remove the prefix
      path.erase(0, this->v_current_dir.length());
      //remove subpath or filename under this directory layer
      if(path.find('/')<path.length()){
        path=path.substr(0, path.find("/")+1);
	  	}
      //store modified path into entries
      entries.push_back(path);
  	}
  }

  //sort entries alphabetically
  std::sort(entries.begin(), entries.end());

  //remove duplicate entry name
  entries.erase(std::unique(entries.begin(), entries.end()), entries.end());

  //find empty name="" and remove from entries
  std::vector<std::string>::iterator empty = find(entries.begin(), entries.end(), "");
  entries.erase(empty);

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

    //print ls format
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

  // actual read
  try {
    std::cout << this->filesystem_service.readFile(filepath, this->private_key) << std::endl;
  } catch (FilesystemService::ReadFileException &ex) {
    std::cout << "Unable to read file" << std::endl;
    return;
  }
}

void Efs::CLI::share(std::string filename, std::string targetuser) {
  // set some initial variables
  std::string filepath_current = this->v_current_dir  + filename;
  std::string dirpath_shared = "/" + targetuser + "/shared/" + this->username + "/";

  // error check
  if (this->username == targetuser) {
    std::cout << "Target user cannot be current user" << std::endl;
    return;
  }
  if (!database->doesFileExist(filepath_current)) {
    std::cout << "File does not exist" << std::endl;
    return;
  }
  if (!database->doesUserExist(targetuser)) {
    std::cout << "User does not exist" << std::endl;
    return;
  }

  // Check target dir
  if (!database->doesDirExist(dirpath_shared)) {
    bool res = this->filesystem_service.createDirectory(dirpath_shared);
    if (!res) {
      throw std::runtime_error("Error with database and actual dir");
    }
  }
  // decrypt current file content
  std::string contents;
  try {
    this->filesystem_service.readFile(filepath_current, private_key);
  } catch (FilesystemService::ReadFileException &ex) {
    std::cout << "Unable to read file" << std::endl;
    return;
  }
  std::cout << "File decrypted" << std::endl;

  // make new file
  // mkfile()
  // add to database
  // this->database->addSharedFileForFile(database->getSha256FromFilePath(v_filepath_current),
  //                                 database->getSha256FromFilePath(v_filepath_shared));

}


void Efs::CLI::mkdir(std::string dirname) {
  if (dirname.back() != '/') dirname += "/";

  // set some initial variables
  std::string dirpath = this->v_current_dir + dirname;

  // case: If dirname is . or ..
  if(dirname == "." || dirname == ".."){
  	std::cout << "Cannot use . or .. for a directory name" << std::endl;
    return;
  }

  // case: if directory already exists
  if (this->database->doesDirExist(dirname)) {
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
  // set some initial variables
  std::string filepath = this->v_current_dir + filename;
  std::string public_key = this->database->getPublicKeyForUser(this->username);

  // case: If dirname is . or ..
  if (filename == "." || filename == "..") {
    std::cout << "Cannot use . or .. for a filename" << std::endl;
    return;
  }

  // cases: if file already exists and if it doesn't
  if (this->database->doesFileExist(filepath)) {
    std::cout << "File already exists" << std::endl;
    // TODO: Remove old file
    // TODO: Replace new file
    // TODO: Re-share with all shared users
    return;
  }

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
