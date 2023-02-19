#include <efs/cli.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <fstream>

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

  // Resolve any symbolic links in the current directory
  std::filesystem::path dir_path = std::filesystem::canonical(currentDir);

  // List the contents of the directory
  for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
    std::cout << entry.path() << std::endl;
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

void Efs::CLI::mkfile(std::string currentDir, std::string filepath, std::string contents) {
  // TODO: Implement me!
  std::cout << "TODO: IMPLEMENT ME!" << std::endl;
}
