#include <efs/cli.h>

Efs::CLI::CLI(Database* database, std::string username, std::string initial_dir) {
  this->database = database;
  this->username = username;
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

void Efs::CLI::ls(std::string currentDir) {
  DIR* dir;
  struct dirent* ent;
  struct stat fileStat;
  std::vector<std::string> entries;

  dir = opendir(currentDir.c_str());
  if (dir == nullptr) {
    std::cerr << "Error: unable to open directory" << std::endl;
  }

  while ((ent = readdir(dir)) != nullptr) {
    // Get information about the file system entry
    std::string entryPath = std::string(currentDir.c_str()) + "/" + std::string(ent->d_name);

    if (stat(entryPath.c_str(), &fileStat) == -1) {
      std::cerr << "Error: unable to get file information for " << ent->d_name << std::endl;
      continue;
    }
    //store every entry into entrires
    entries.push_back(ent->d_name);
  }

	//sort entries alphabetically
  std::sort(entries.begin(), entries.end());
  std::cout << "d -> . " << std::endl;
  std::cout << "d -> .." << std::endl;
  // iterate through directory entries
  for (const auto& entry : entries) {
    std::string entryPath = std::string(currentDir.c_str()) + "/" + entry;
    if (stat(entryPath.c_str(), &fileStat) == -1) {
      std::cerr << "Error: unable to get file information for " << entry << std::endl;
      continue;
    }

    // Determine the type of the file system entry
    char fileType;
    switch (fileStat.st_mode & S_IFMT) {
      case S_IFDIR:
        fileType = 'd';
        break;
      case S_IFREG:
        fileType = 'f';
        break;
      default:
        fileType = '?';
        break;
    }

    //reference the entire filepath:/admin/folder
    std::string filepath = this->database->getFilepathFromSha256(entry);

    //check and print entries existed in database
    if(entry.length()==64 && filepath != "") {
      std::cout << fileType << " -> "<< Utils::getFilenameFromFilepath(filepath) << std::endl;
    }
  }
}

std::string Efs::CLI::cat(std::string filename, std::string private_key) {
  // construct the filepath
  std::string filepath = this->v_current_dir + filename;

  // Check if the file exists
  if (!this->database->doesFileExist(filepath)) {
    std::cout << "File does not exist: " << filepath << std::endl;
    return "";
  }

  // actual read
  try {
    return this->filesystem_service.readFile(filepath, private_key);
  } catch (FilesystemService::ReadFileException &ex) {
    std::cout << "Unable to read file" << std::endl;
    return "";
  }
}

void Efs::CLI::share(std::string currentDir, std::string filepath, std::string targetuser) {
  // TODO: Implement me!
  std::cout << "TODO: IMPLEMENT ME!" << std::endl;
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
