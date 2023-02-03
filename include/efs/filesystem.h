#include <nlohmann/json.hpp>

namespace Efs {
  class Filesystem {
    public:
      Filesystem();
      ~Filesystem();
      void addFile();
      void deleteFile();
      void shareFile();
      void updateFile();
      void doesFileExist();
      void doesDirectoryExist();
  };
}
