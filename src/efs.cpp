#include <sstream>
#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>

#include <efs/efs.h>
#include <efs/cli.h>
#include <efs/database.h>

Efs::Efs::Efs(int argc, char** argv) {
  Database database;

  // initial check if admin user is there. If not then create the admin user
  if (!database.doesUserExist("admin")) {
    database.createUser("admin");
    std::cout << "Created admin! Since this is first run, exiting!" << std::endl;
    return;
  }

  // if keyfile not inputted, ask for keyfile
  if (argv[1] == NULL) {
    std::cout << "Please enter keyfile!" << std::endl;
    std::cout << "Example: ./fileserver <keyfile_name>" << std::endl;
    return;
  }

  // get the keyfile name
  std::string keyfile_name = argv[1];

  /* KEEP TRACK OF VARIABLES HERE */
  // create a new CLI object
  CLI cli;
  std::string currentDir = "/";
  std::string currentUser;

  // Open and read the key file
  std::ifstream infile(keyfile_name);
  if (!infile) {
    std::cerr << "Error: Could not open key file '" << keyfile_name << "'" << std::endl;
    return;
  }
  std::string privateKeyStr((std::istreambuf_iterator<char>(infile)),
                            std::istreambuf_iterator<char>());
  if (infile.fail() && !infile.eof()) {
        std::cerr << "Error: Failed to read data from file '" << keyfile_name << "'" << std::endl;
        infile.close();
        return;
  }
  infile.close();
  // std::string privateKeyStr = "-----BEGIN RSA PRIVATE KEY-----\nMIIEpAIBAAKCAQEA0EhDZHTDY9y5bl5nedrD8lB9AnBRcwKHBlOdyV/LITeGNpID\nCK7tg+r6styVVGY7TRXi5wcOe2bSN+QUTEUyLFBhlry4Al6lSKJV6w7TsWxHUXxN\nlcVNiKny5lrFw2ABYp16FaxgBA/SsYKswUdslsBFf6mVjZcSrC+xbMOAuvVPNUkF\nU8BFj9EBYBUNDreFPHlByabZUF/74GxnNHU63wE+kDfeiN93/SeLz52IqXq3GzLF\nYrye1lthg5VHrto7Jl+B6i5VBkFeHWUqMgZoobdcEnD4xwTkSmOVZF6hDR6FGRy8\nc/ck9s3bNWtOLFQkkxc4VddPyVDYslEBXZp0BwIDAQABAoIBABV6YN9i6Q6LDDYq\nQa+/qaAlwQEM4GFPRpRA8CRRdArSBSLrI1P0Z1AeCEnZ1FkBN9FHAKsulPA7FfzR\n0MQICK39gKrA0uraTrSy73rkqwYrR8l1MNz7x8yK1fOqMvrm6LhTdpZLPGrUJOJ4\nOroC/wV8AKQIXVXeA9IXaEudIiZlML+D7fP0FcDSe0Rab0RyhuBYskY4DZGgC+/+\n/uGFYBYzB0xgWgXYViVrT9/a46Rp1Pg7hJ0wlNxqFAfSEM0r/eYSDyVF1lcPVXLZ\nalk8f5++tv3alf05CMwejjBFR9oJv7yDhT5L9iajoNGuEZS7RMdhhWPXL09uw1tw\nObQ8CuECgYEA+7l8m2V1WjCyzW2veobv7vkeu4YIlTa9NjyX8QDxr2x+uMZvMWoj\nYG5080QtsuV3t3XMABIE0bPXpk3gRNTlzJ64M7lcKtGDe/x2NjNoqcEhqoDXG8NG\nQlB9zvgiAT/jzn2TYLb6I6hB1iSUDt7lwY9fl+hdpqcWoL2dWzZji2ECgYEA09Hj\nDGCzpMxS3y8MO+JW/R5hP/sXOAjuwdM4cDJUcWu80fZekLcUyEPCgC5zPoyrfMY0\nwYNbM5TqLhqc47ezOY1eZAQZDAAjfXL0jUbnTSolzkkbtAxZTxuNpEYv+vKzmoJw\nBPsVv8U70Qt0FhPaU2Ig3cLAzNMudvsP/szpYGcCgYA5bBi2wjBBALbpEcpj4wA8\nvWt7T5oJn9EN3Ei024hGgjNuJyYaWaH0T34yOC1Mj/f9S8vbgEDFLYdWEpHEYeSC\nBvryIaiaw+yAjhjRSGzpvHrXkEiBtswCOKZaXlQzgMWQxyileVgaZjmOhg2YTtSO\nkQw3dPRYHemVhWut8PxIQQKBgQC+Cfq4ajICUsavMhi5yXHeGlmJ+45zWaBMCWMD\nVkRolg6MGYvyLi/ZFOH1+raukapxgapnS6CRgLMxtfK4Y2HK6sia1wpZt+4faPl7\nnvd6RawxjWnreYDO9pt3nPkDHqiAoTI7ZcG3kg7+mFYIULt6P/nYvBgaLAXt/YX2\nRoXDVQKBgQCzrxHzYNMUUWEKx/ro1XihFxaT5SX1ozj/u3ju4uRjVzzIbfr1rNqR\nHspCPITNdd3xaJkdQPGkmpRM10yEfvWViAq4rKeUm+uK0XI6V1uZQyOFpXVF2264\n70oaZ2idznUuC7fMQhaEkbV6QaODrtr1agT65m4YbT+iVPjHKvkVlQ==\n-----END RSA PRIVATE KEY-----\n"; // private key in PEM format
  std::string publicKeyStr = "-----BEGIN RSA PUBLIC KEY-----\nMIIBCgKCAQEA0EhDZHTDY9y5bl5nedrD8lB9AnBRcwKHBlOdyV/LITeGNpIDCK7t\ng+r6styVVGY7TRXi5wcOe2bSN+QUTEUyLFBhlry4Al6lSKJV6w7TsWxHUXxNlcVN\niKny5lrFw2ABYp16FaxgBA/SsYKswUdslsBFf6mVjZcSrC+xbMOAuvVPNUkFU8BF\nj9EBYBUNDreFPHlByabZUF/74GxnNHU63wE+kDfeiN93/SeLz52IqXq3GzLFYrye\n1lthg5VHrto7Jl+B6i5VBkFeHWUqMgZoobdcEnD4xwTkSmOVZF6hDR6FGRy8c/ck\n9s3bNWtOLFQkkxc4VddPyVDYslEBXZp0BwIDAQAB\n-----END RSA PUBLIC KEY-----\n";
  // Validate keyfile here to get who the current user is
  currentUser = database.getUsernameByPrivateKey(privateKeyStr);
  if (currentDir == "") {
    std::cout << "No record found!" << std::endl;
    return;
  } else {
    std::cout << "Welcome back, " + currentUser + "!" << std::endl;
  }

  // TODO: Login as the current user and update the current dir

  // main loop
  while (true) {
    std::string cmd;
    std::cout << "(EFS)$ ";
    std::getline(std::cin, cmd);

    // get command into a vector
    std::vector<std::string> v_cmd;
    std::string tmp;
    std::stringstream ss(cmd);

    // if cmd is empty, just loop again
    if (cmd.empty()) {
      continue;
    }

    while(getline(ss, tmp, ' ')) {
      v_cmd.push_back(tmp);
    }
  
    // Check commands
    if (v_cmd[0] == "cd") {
      if (v_cmd.size() != 2) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: cd <directory>\n";
        continue;
      }

      // declare and initialize targetDir
      std::string targetDir;

      // get target directory from command line arguments
      if (v_cmd[1][0] == '/') {
        targetDir = v_cmd[1];
      } else {
        targetDir = currentDir + "/" + v_cmd[1];
      }

      // check whether target directory exists
      if (!std::filesystem::exists(targetDir)) {
        std::cout << "Directory does not exist\n";
        continue;
      }

      // normalize target directory path
      targetDir = std::filesystem::path(targetDir).lexically_normal().string();
      currentDir = targetDir;
    } else if (v_cmd[0] == "pwd") {
      if (v_cmd.size() != 1) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: pwd\n";
        continue;
      }
      cli.pwd(currentDir);
    } else if (v_cmd[0] == "ls") {
      if (v_cmd.size() != 1) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: ls\n";
        continue;
      }
      cli.ls(currentDir);
    } else if (v_cmd[0] == "cat") {
      if (v_cmd.size() != 2) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: cat <filename>\n";
        continue;
      }
      cli.cat(currentDir, v_cmd[1]);
    } else if (v_cmd[0] == "share") {
      if (v_cmd.size() != 3) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: share <filename> <username>\n";
        continue;
      }
      cli.share(currentDir, v_cmd[1], v_cmd[2]);
    } else if (v_cmd[0] == "mkdir") {
      if (v_cmd.size() != 2) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: mkdir <directory>\n";
        continue;
      }
      cli.mkdir(currentDir, v_cmd[1]);
    } else if (v_cmd[0] == "mkfile") {
      if (v_cmd.size() != 3) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: mkfile <filename> <contents>\n";
        continue;
      }
      cli.mkfile(currentDir, v_cmd[1], v_cmd[2]);
    } else if (v_cmd[0] == "exit") {
      std::cout << "Exiting" << std::endl;
      return;
    } else {
      std::cout << "Please refer to user manual for valid commands\n";
    }
  }
}
