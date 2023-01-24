# Encrypted FS

## ⌨️ Getting started

**Configuring the project (only need to run on first time):**

```sh
mkdir -p build
cmake . -B build              # Configures the cmake project
```

**Building the project:**

```sh
cmake --build build           # Compiles and builds the Spotter application
```

**Running the application:**

```sh
./build/main
```

## ⚒️ Development

### Visual Studio Code

To set up your environment on visual studio code, follow the steps below:

- Be sure to have the [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extension
- Put the following in `.vscode/c_cpp_properties.json` file:
  
    ```json
    {
      "configurations": [
        {
          "name": "CMake",
          "compileCommands": "${config:cmake.buildDirectory}/compile_commands.json",
          "configurationProvider": "ms-vscode.cmake-tools"
        }
      ],
      "version": 4
    }
    ```

    and have the following `.vscode/settings.json` file:

    ```json
    {
      "editor.indentSize": "tabSize",
      "editor.tabSize": 2
    }
    ```
