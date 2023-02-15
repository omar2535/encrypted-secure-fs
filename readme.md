# Encrypted FS

## ⌨️ Getting started

**Cloning the repository (only once):**

```sh
git clone https://github.com/omar2535/encrypted-secure-fs.git
```

**Installing required libraries (only once):**

```sh
sudo apt-get -y install cmake # for mac users do brew install cmake
sudo apt-get -y install libssl-dev # for mac users do brew install openssl
```

**Configuring the project (only once):**

```sh
git submodule update --init --recursive
mkdir -p build
cmake . -B build              # Configures the cmake project
```

**Building the project (run every time you change the code):**

```sh
cmake --build build           # Compiles and builds the Spotter application
```

**Running the application (run when you want to try running the application):**

```sh
./build/fileserver <keyfile>
```

## ⚒️ Development

### Visual Studio Code

To set up your environment on visual studio code, follow the steps below:

- Be sure to have the [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extension
