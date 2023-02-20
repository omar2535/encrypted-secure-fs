#include <efs/cli.h>
#include <efs/database.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <stdlib.h>
#include <dirent.h>

std::string Efs::CLI::cd(std::string currentDir, std::string targetDir) {
  // Check if the target directory is an absolute path or a relative path
  if (targetDir[0] != '/') {
    // Target directory is a relative path, so add the current directory to the beginning
    targetDir = currentDir + "/" + targetDir;
  }

  // Normalize the target directory path by removing any redundant separators and resolving any relative paths
  targetDir = std::filesystem::path(targetDir).lexically_normal().string();
  currentDir = targetDir;

  // Set the current directory to the root directory of the system if the target directory is an absolute path
  if (targetDir[0] == '/') {
    m_currentDir = "/";
  }

  // Split the target directory path into its components
  std::vector<std::string> components;
  std::stringstream ss(targetDir);
  std::string component;
  while (std::getline(ss, component, '/')) {
    if (component == "..") {
      // If the component is "..", remove the last component from the current directory
      if (components.size() > 0) {
        components.pop_back();
      }
    } else if (component != "." && component != "") {
      // If the component is not "." or an empty string, add it to the list of components
      components.push_back(component);
    }
  }

  // Traverse the directory tree to the target directory
  for (const auto& component : components) {
    // Check if the current directory is valid
    if (!std::filesystem::is_directory(m_currentDir)) {
      std::cout << "Invalid directory: " << m_currentDir << std::endl;
      return "";
    }

    // Try to move to the target directory
    std::filesystem::path newPath = m_currentDir;
    newPath /= component;
    if (std::filesystem::is_directory(newPath)) {
      m_currentDir = newPath.string();
    } else if (!std::filesystem::exists(newPath)) {
      std::cout << "Directory does not exist: " << component << std::endl;
      return "";
    } else {
      std::cout << "Path is not a directory: " << component << std::endl;
      return "";
    }
  }

  // Check if the target directory exists
  if (!std::filesystem::is_directory(m_currentDir)) {
    std::cout << "Directory does not exist: " << targetDir << std::endl;
    return "";
  }


  return m_currentDir;
}

void Efs::CLI::pwd(std::string currentDir) {
  std::cout << currentDir << std::endl;

  // Update m_currentDir to the current working directory
  m_currentDir = currentDir;
}

void Efs::CLI::ls(std::string currentDir) {
  // Check if the current directory exists
	if (!std::filesystem::exists(currentDir)) {
		std::cout << "Invalid directory: " << currentDir << std::endl;
		return;
	}

	struct dirent **namelist;
	int n=scandir(".", &namelist, NULL, alphasort);;
	int i=0;

	// print all the files and directories within directory */
	while (n--) {
		std::cout << namelist[i]->d_name << std::endl;
		i++;
		free(namelist[n]);
	}
	free(namelist);
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

void Efs::CLI::mkdir(std::string currentDir, std::string dirpath) {
  // Normalize the directory path by removing any redundant separators and resolving any relative paths
  dirpath = std::filesystem::path(dirpath).lexically_normal().string();

  // Construct the directory path relative to the current directory
  std::filesystem::path newDir = std::filesystem::path(currentDir) / dirpath;

  // Try to create the directory
  if (!std::filesystem::create_directory(newDir)) {
    std::cout << "Could not create directory: " << dirpath << std::endl;
    return;
  }
}

// makes the file with the given contents
// if already exists, overwrite the file and re-share with all shared users
void Efs::CLI::mkfile(std::string currentUser, std::string r_currentDir,
                      std::string v_currentDir, std::string v_filename,
                      std::string contents) {
  // make sure all dir names end with slash
  if (v_currentDir.back() != '/') v_currentDir += "/";
  if (r_currentDir.back() != '/') r_currentDir += "/";

  // set some initial variables
  Database database;
  std::string v_filepath = v_currentDir + v_filename;
  std::string public_key = database.getPublicKeyForUser(currentUser);

  // cases: if file already exists and if it doesn't
  if (database.doesFileExist(v_filepath)) {
    std::cout << "File already exists" << std::endl;
    // TODO: Remove old file
    // TODO: Replace new file
    // TODO: Re-share with all shared users
  } else {
    // 1. Register file to database
    std::string r_filename = database.addFile(v_filepath);

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
