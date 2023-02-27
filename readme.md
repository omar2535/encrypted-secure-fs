# Encrypted FS

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)

## 🪟 Overview

This project is a simulation of a filesystem where all files are encrypetd. Users may use this program as a way to manage their files and directories without having other people know what the contents of the files are (with the exception of the admin).

Currently, the supported commands are:

| Command | Description |
| --- | --- |
| cd <directory> | Move the directory up or down (IE. `cd ..`) |
| pwd | Prints the present working directory |
| ls | Lists all files in the current directory |
| cat <filename> | Outputs contents in a file |
| share <filename> <username> | Shares a file with a user (IE. `share myfile.txt bob`) |
| mkdir <dirname> | Creates a directory (IE. `mkdir mydir`) |
| mkfile <filename> <content> | Creates a file with the given filename and contents (IE. `mkfile hello.txt hello world!`) |
| exit | Terminates the program |

There are also **admin** specific features in addition to the ones presented above.

| Command | Description |
| --- | --- |
| addduser <username> | Creates a user with the given username (IE. `adduser bob`) |

## ⌨️ Getting started

**1. Cloning the repository (only once):**

```sh
git clone https://github.com/omar2535/encrypted-secure-fs.git
```

**2. Installing required libraries (only once):**

*Ubuntu:*

Ubuntu steps are pretty straight-forward

```sh
sudo apt-get -y install cmake
sudo apt-get -y install libssl-dev
```

*Apple:*

Mac users will need some extra steps to search for the library paths when using cmake

```sh
brew install cmake
brew install openssl

ln -s /usr/local/opt/openssl/include/openssl /usr/local/include # link openssl lib to `/usr/local/include`
echo "export OPENSSL_ROOT_DIR=/usr/local/opt/openssl" >> ~/.zshrc
echo "export C_INCLUDE_PATH=/usr/local/include" >> ~/.zshrc
echo "export CPLUS_INCLUDE_PATH=/usr/local/include" >> ~/.zshrc
```

**3. Configuring the project (only once):**

```sh
git submodule update --init --recursive
mkdir -p build
cmake . -B build              # Configures the cmake project
```

**4. Building the project (run every time you change the code):**

```sh
cmake --build build           # Compiles and builds the Spotter application
```

**5. Running the application (run when you want to try running the application):**

```sh
./build/fileserver <keyfile>
```

## ⚒️ Development

### Visual Studio Code

To set up your environment on visual studio code, follow the steps below:

- Be sure to have the [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extension

### Contribution

To contribute, create a new branch, make changes, commit and make a pull request.

```sh
git checkout -b <my_branch_name>
# Make changes
git add .
git commit -m <my_commit_message>
git push --set-upstream origin <my_branch_name>
```

## 🎬 Demo

https://user-images.githubusercontent.com/9442294/221485990-7139fb6a-2a1e-46e2-91e8-06d47bae0b73.mp4



