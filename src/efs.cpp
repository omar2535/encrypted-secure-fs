#include <sstream>
#include <iostream>
#include <vector>
#include <assert.h>

#include <efs/efs.h>
#include <efs/cli.h>
#include <efs/database.h>
#include <efs/admin.h>

Efs::Efs::Efs(int argc, char** argv) {
  // initial loop for admin creation and database creation
  Database database;
  if (!database.isDatabaseInitialized()) {
    std::cout << "No initial database found! Initializing database and admin user" << std::endl;
    Admin admin;
    admin.initilizeAdmin();
    std::cout << "Admin user created!" << std::endl;
    // TODO: Run database initialization
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

  // TODO: Validate keyfile here to get who the current user is
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
      cli.cd(currentDir, v_cmd[1]);
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
