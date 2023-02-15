# Encrypted FS

## ⌨️ Getting started

**Cloning the repository:**

```sh
git clone https://github.com/omar2535/encrypted-secure-fs.git
```

**Installing required libraries:**

```sh
sudo apt-get -y install cmake
sudo apt-get install libssl-dev
```

**Configuring the project (only need to run on first time):**

```sh
git submodule update --init --recursive
mkdir -p build
cmake . -B build              # Configures the cmake project
```

**Building the project:**

```sh
cmake --build build           # Compiles and builds the Spotter application
```

**Running the application:**

```sh
./build/fileserver <keyfile>
```

## ⚒️ Development

### Visual Studio Code

To set up your environment on visual studio code, follow the steps below:

- Be sure to have the [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extension
