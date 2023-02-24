#include <efs/efs.h>



Efs::Efs::Efs(int argc, char** argv) {
  Database database;
  UserManager userManager(&database);

  // initial check if admin user is there. If not then create the admin user
  if (!database.doesUserExist("admin")) {
    userManager.createUser("admin");
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

  // login
  std::string username = loginUser(keyfile_name, database);
  if (username.empty()) {
    std::cout << "User not found!" << std::endl;
    return;
  } else {
    std::cout << "Welcome back, " + username + "!" << std::endl;
  }

  // intialize dirs
  std::string r_currentDir = (std::string) std::filesystem::current_path(); // TODO: Remove this
  std::string v_currentDir = "/" + username + "/";

  // initialize the user's CLI
  CLI cli(&database, username, v_currentDir);

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
      // Call the cd function and store the returned vector in a variable
      std::vector<std::string> result = cli.cd(v_currentDir, v_cmd[1]);
      // Extract the individual elements from the vector and store them in new string variables
      r_currentDir = result[0];
      v_currentDir = result[1];
    } else if (v_cmd[0] == "pwd") {
      if (v_cmd.size() != 1) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: pwd\n";
        continue;
      }
      cli.pwd(v_currentDir);
    } else if (v_cmd[0] == "ls") {
      if (v_cmd.size() != 1) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: ls\n";
        continue;
      }
      cli.ls(r_currentDir);
    } else if (v_cmd[0] == "cat") {
      if (v_cmd.size() != 2) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: cat <filename>\n";
        continue;
      }
      cli.cat(r_currentDir, v_cmd[1]);
    } else if (v_cmd[0] == "share") {
      if (v_cmd.size() != 3) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: share <filename> <username>\n";
        continue;
      }
      cli.share(r_currentDir, v_cmd[1], v_cmd[2]);
    } else if (v_cmd[0] == "adduser") {
      if (v_cmd.size() != 2) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: adduser <username>";
        continue;
      }
      cli.adduser(v_cmd[1]);
    } else if (v_cmd[0] == "mkdir") {
      if (v_cmd.size() != 2) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: mkdir <directory>\n";
        continue;
      }
      cli.mkdir(username, r_currentDir, v_currentDir, v_cmd[1]);
    } else if (v_cmd[0] == "mkfile") {
      if (v_cmd.size() < 3) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: mkfile <filename> <contents>\n";
        continue;
      }
      // get the last argument as a vector slice so that we can have spaces in the file contents
      std::vector v_file_contents = std::vector<std::string>(v_cmd.begin()+2, v_cmd.end());

      // combine the last file_contents ivector nto a string delimited with space
      std::string file_contents = Utils::join(v_file_contents, " ");

      // call makefile on the combined file-contents
      cli.mkfile(v_cmd[1], v_cmd[2]);
    } else if (v_cmd[0] == "exit") {
      std::cout << "Exiting" << std::endl;
      return;
    } else {
      std::cout << "Please refer to user manual for valid commands\n";
    }
  }
}
