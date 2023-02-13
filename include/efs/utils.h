namespace Efs {
  class Utils {
    std::string getFilenameFromFilepath(std::string filepath) {
      std::string base_filename = path.substr(path.find_last_of("/\\") + 1)
      return base_filename;
    }
  }
}
