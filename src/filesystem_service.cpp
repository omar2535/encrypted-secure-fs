#include <efs/filesystem_service.h>

Efs::FilesystemService::FilesystemService() {
  // make sure the filesystem root dir exists. If not, make it
  if (!std::filesystem::is_directory(this->R_ROOT_DIR)) {
    std::filesystem::create_directory(this->R_ROOT_DIR);
  }
}

std::string Efs::FilesystemService::readFile(std::string v_filepath, std::string private_key) {
  try {
    // Convert virtual filepath to real filepath
    std::filesystem::path r_filepath = this->convertVirtualPathToRealPath(v_filepath);

    // Decrypt the file
    return Crypto::decryptFile(private_key, r_filepath);
  } catch (const std::exception& ex) {
    std::cerr << "Encountered error:" << ex.what() << std::endl;
    throw ReadFileException("Unable to read file");
  }
}

bool Efs::FilesystemService::createDirectory(std::string v_dirpath) {
  try {
    std::filesystem::path r_dirpath = this->convertVirtualPathToRealPath(v_dirpath);

    if (!std::filesystem::is_directory(r_dirpath)) {
      std::filesystem::create_directory(r_dirpath);
      return true;
    }
    return false;
  } catch (const std::exception& ex) {
    std::cerr << "Encountered error:" << ex.what() << std::endl;
    throw CreateDirectoryException("Unable to create directory");
  }
}

bool Efs::FilesystemService::createFile(std::string v_filepath, std::string contents, std::string public_key) {
  try {
    std::filesystem::path r_filepath = this->convertVirtualPathToRealPath(v_filepath);

    if (!std::filesystem::exists(r_filepath)) {
      std::ofstream outfile(r_filepath);
      outfile << contents << std::endl;
      outfile.close();
      Crypto::encryptFile(public_key, r_filepath);
      return true;
    }
    return false;
  } catch (const std::exception& ex) {
    std::cerr << "Encountered error:" << ex.what() << std::endl;
    throw CreateFileException("Unable to create file");
  }
}

void Efs::FilesystemService::shareFile(std::string private_key, std::string v_filepath, std::string target_dir) {
  try {
    // Create target dir

  } catch (const std::exception& ex) {
    std::cerr << "Encountered error:" << ex.what() << std::endl;
    throw ShareFileException("Unable to share file");
  }
}

/* PRIVATE FUNCTIONS HERE */

std::filesystem::path Efs::FilesystemService::convertVirtualPathToRealPath(std::string v_path) {
  // if path is just "/", return the root
  if (v_path == "/") return this->R_ROOT_DIR;

  // intialize our path tracking building variables
  std::string v_curr_built_path = "/";
  std::filesystem::path r_curr_built_path = this->R_ROOT_DIR;

  // split the virtual path by '/': IE: /admin/asdf -> ['', 'admin', 'asdf]
  std::vector<std::string> split_vpath = Utils::splitString(v_path, '/');

  // iterate through the split virtual path and update both trackers (except for the last one)
  // IE. /admin/personal/shared ->
  // 1. admin
  // 2. personal
  for (int i=0; i<split_vpath.size()-1; i++) {
    if (split_vpath[i] == "") continue;

    v_curr_built_path += split_vpath[i] + "/";
    r_curr_built_path /= Crypto::getSha256ForString(v_curr_built_path);
  }

  // if it's a file, do the last one without an appended slash. If it isn't, append
  if (v_path.back() == '/') {
    v_curr_built_path += split_vpath.back() + "/";
  } else {
    v_curr_built_path += split_vpath.back();
  }

  r_curr_built_path /= Crypto::getSha256ForString(v_curr_built_path);
  return r_curr_built_path;
}
