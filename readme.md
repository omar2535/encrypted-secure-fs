# Encrypted FS

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
