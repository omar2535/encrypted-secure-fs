#pragma once

#include <filesystem>
#include <vector>

#include <efs/crypto.h>
#include <efs/utils.h>

#define FS_FOLDER_NAME "FILESYSTEM"

namespace Efs {
  class FilesystemService {
    public:
      /**
       * @brief Construct a new Filesystem Service object
      */
      FilesystemService();

      /**
       * @brief Get the contents of a file given its virtual filepath
       * @param v_filepath virtual filepath
       * @param priavte_key the private key for the user
       * @throws ReadFileException when fileread fails
       */
      std::string readFile(std::string v_filepath, std::string private_key);

      /**
       * @brief Create the directory on the underlying filesystem given the virtual filepath
       * @param v_dirpath virtual folderpath
       * @throws CreateDirectoryException when creating a directory fails
       * @returns true if creating directory was successful, false if it already existed
       */
      bool createDirectory(std::string v_dirpath);

      /**
       * @brief Create the file contents to the file for a virtual filepath
       *
       * @param v_filepath virtual filepath
       * @param contents file contents
       * @param public_key the public key for the user
       * @throws CreateFileException when creating a file fails
       * @returns returns true if creating a file succeeded, false if it already existed
       */
      bool createFile(std::string v_filepath, std::string contents, std::string public_key);

      /**
       * @brief Copies a file from the source filepath to the destination filepath
       *
       * @param v_src_filepath the virtual source filepath
       * @param v_dst_filepath the virtual destination filepath
       * @param src_private_key the private key for the source filepath
       * @param dst_public_key the public key to encrypt the destination file with
       * @return true if successful
       * @return false  if unsuccessful
       */
      bool copyFile(std::string v_src_filepath, std::string v_dst_filepath, std::string src_private_key, std::string dst_public_key);

      /**
       * @brief Delete a file
       *
       * @param v_filepath The virtual filepath
       * @return true if the file is deleted
       * @return false if the file isn't deleted
       */
      bool deleteFile(std::string v_filepath);

      /**
       * @brief Exception when reading a file fails
       *
       */
      class ReadFileException: public std::exception {
        public:
          std::string _msg;
          ReadFileException(const std::string& msg) : _msg(msg){}
          virtual const char* what() const noexcept override {
            return _msg.c_str();
          }
      };

      /**
       * @brief Exception when creating a dir fails
       *
       */
      class CreateDirectoryException: public std::exception {
        public:
          std::string _msg;
          CreateDirectoryException(const std::string& msg) : _msg(msg){}
          virtual const char* what() const noexcept override {
            return _msg.c_str();
          }
      };

      /**
       * @brief Exception when creating a file fails
       *
       */
      class CreateFileException: public std::exception {
        public:
          std::string _msg;
          CreateFileException(const std::string& msg) : _msg(msg){}
          virtual const char* what() const noexcept override {
            return _msg.c_str();
          }
      };

      /**
       * @brief Exception when copying a file fails
       *
       */
      class CopyFileException: public std::exception {
        public:
          std::string _msg;
          CopyFileException(const std::string& msg) : _msg(msg){}
          virtual const char* what() const noexcept override {
            return _msg.c_str();
          }
      };

      /**
       * @brief Exception when deleting a file fails
       *
       */
      class DeleteFileException: public std::exception {
        public:
          std::string _msg;
          DeleteFileException(const std::string& msg) : _msg(msg){}
          virtual const char* what() const noexcept override {
            return _msg.c_str();
          }
      };

    private:
      std::filesystem::path R_ROOT_DIR = std::filesystem::current_path() / FS_FOLDER_NAME;

      /**
       * @brief Converts a virtual filepath to the real filepath
       * Note: Doesn't know whether the path is allowed. Simply just returns
       * a filepath based on the virtual path
       *
       * @param v_path virtual filepath
       * @return std::filesystem::path the real filepath
       */
      std::filesystem::path convertVirtualPathToRealPath(std::string v_path);
  };
}
