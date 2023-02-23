#include <efs/cli.h>

std::vector<std::string> Efs::CLI::cd(std::string currentDir, std::string targetDir, Database* database) {
  std::string pre_r_currectDir = m_currentDir;
  std::string lastPathComponent = std::filesystem::path(targetDir).filename().string();
  // Handle moving up to the parent directory
  if (lastPathComponent == "..") {
    m_currentDir = std::filesystem::path(m_currentDir).parent_path().string();
    v_currentDir = std::filesystem::path(v_currentDir).parent_path().string();
    v_currentDir = std::filesystem::path(v_currentDir).parent_path().string();
    if (v_currentDir == "/") {
      m_currentDir = (std::string) std::filesystem::current_path(); 
      return {m_currentDir, v_currentDir};
    }
    return {m_currentDir, v_currentDir};
    
  }

  std::string entirePath;
  entirePath = database->getSha256FromFilePath(currentDir + lastPathComponent + "/");
  
  // Check if the target directory exists
  if (entirePath == "") {
    std::cout << "1. Directory does not exist: " << targetDir << std::endl;
    return {pre_r_currectDir, v_currentDir};
  }

  // Set the current directory to the root directory of the system if the target directory is an absolute path
  if (entirePath[0] == '/') {
    m_currentDir = "/";
  }
  // Split the target directory path into its components
  std::vector<std::string> components;
  std::stringstream ss(entirePath);
  std::string component;
  while (std::getline(ss, component, '/')) {
    if (component == "..") {
      // If the component is "..", remove the last component from the list of components
      if (components.size() > 0 && m_currentDir != "/") {
        components.pop_back();
      }
    } else if (component != "." && component != "") {
      // If the component is not "." or an empty string, add it to the list of components
      components.push_back(component);
    }
  }

  // Traverse the directory tree to the target directory
  for (const auto& component : components) {
    // Try to move to the target directory
    std::filesystem::path newPath = m_currentDir;
    newPath /= component;
    if (std::filesystem::is_directory(newPath)) {
      m_currentDir = newPath.string();
    } else {
      std::cout << "2. Directory does not exist: " << targetDir << std::endl;
      return {pre_r_currectDir, v_currentDir};
    }
  }

  v_currentDir = currentDir + lastPathComponent + "/";
  return {m_currentDir, v_currentDir};
}

void Efs::CLI::pwd(std::string currentDir, Database* database) {
  std::cout << currentDir << std::endl;
}

void Efs::CLI::ls(std::string currentDir, Database* database) {
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
    std::string filepath = database->getFilepathFromSha256(entry);

    //check and print entries existed in database
    if(entry.length()==64 && filepath != "") {
      std::cout << fileType << " -> "<< Utils::getFilenameFromFilepath(filepath) << std::endl;
    }
  }
}

void Efs::CLI::cat(std::string currentDir, std::string filepath) {
  // Check if the file exists
  std::filesystem::path filePath = currentDir + "/" + filepath;
  if (!std::filesystem::exists(filePath)) {
    std::cout << "File does not exist: " << filepath << std::endl;
    return;
  }

  // Check if the file is readable
  std::ifstream fileStream(filePath);
  if (!fileStream.is_open()) {
    std::cout << "Could not open file: " << filepath << std::endl;
    return;
  }

  // Read the contents of the file and output to the console
  std::string line;
  while (std::getline(fileStream, line)) {
    std::cout << line << std::endl;
  }
}

void Efs::CLI::share(std::string currentDir, std::string filepath, std::string targetuser) {
  // TODO: Implement me!
  std::cout << "TODO: IMPLEMENT ME!" << std::endl;
}

void Efs::CLI::mkdir(std::string currentUser, std::string r_currentDir,
                     std::string v_currentDir, std::string v_dirname,
                     Database* database) {
  // make sure all dir names end with slash
  if (v_currentDir.back() != '/') v_currentDir += "/";
  if (r_currentDir.back() != '/') r_currentDir += "/";
  if (v_dirname.back() != '/')    v_dirname += "/";

  // set some initial variables
  std::string v_dirpath = v_currentDir + v_dirname;
  std::string public_key = database->getPublicKeyForUser(currentUser);

  // cases: if directory already exists and if it doesn't
  if (database->doesDirExist(v_dirpath)) {
    std::cout << "Directory already exists" << std::endl;
  }
  else if(v_dirname=="./" || v_dirname=="../"){
  	std::cout << "Cannot use . or .. for a directory name" << std::endl;
  }
  else {
    // 1. Register directory to database and encrypt the directory path
    //    with the current user's public key
    std::string r_dirname = database->addDir(v_dirpath);

    // 2. Construct the actual path of the directory on the operating system
    std::string r_dirpath = r_currentDir + r_dirname;

    // 3. On operating system, create the directory in the current directory
    //    with the hashed directory name
    std::filesystem::create_directory(r_dirpath);

    // 4. Print out success
    std::cout << "Created directory: " << v_dirname << std::endl;
  }
}

// makes the file with the given contents
// if already exists, overwrite the file and re-share with all shared users
void Efs::CLI::mkfile(std::string currentUser, std::string r_currentDir,
                      std::string v_currentDir, std::string v_filename,
                      std::string contents, Database* database) {
  // make sure all dir names end with slash
  if (v_currentDir.back() != '/') v_currentDir += "/";
  if (r_currentDir.back() != '/') r_currentDir += "/";

  // set some initial variables
  std::string v_filepath = v_currentDir + v_filename;
  std::string public_key = database->getPublicKeyForUser(currentUser);

  // cases: if file already exists and if it doesn't
  if (database->doesFileExist(v_filepath)) {
    std::cout << "File already exists" << std::endl;
    // TODO: Remove old file
    // TODO: Replace new file
    // TODO: Re-share with all shared users
  }
  else if(v_filename=="." || v_filename==".."){
  	std::cout << "Cannot use . or .. for a file name" << std::endl;
  }
  else {
    // 1. Register file to database
    std::string r_filename = database->addFile(v_filepath);

    // 2. Construct the actual filepath on the operating system
    std::string r_filepath = r_currentDir + r_filename;

    // 3. On operating system, create the file in the current directory
    //    with the hashed filename
    std::ofstream outfile(r_filepath);
    outfile << contents << std::endl;
    outfile.close();

    // 4. Encrypt the file
    Crypto::encryptFile(public_key, r_filepath);

    // 5. Print out success
    std::cout << "Created file: " << v_filename << std::endl;
  }
}
