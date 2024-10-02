![Logo](logo.png)
# cBudgie Programming Language

Welcome to the official repository for the `cBudgie` programming language! `cBudgie` is a modern, lightweight, and easy-to-learn language designed for simplicity and efficiency. This repository contains the source code, documentation, and tools related to `cBudgie`.

## Introduction

`cBudgie` is designed to be a user-friendly language for beginners and a powerful tool for experienced developers. It combines the best features of various programming languages, offering a clean syntax and robust functionality.

## Features

- **Simple Syntax:** Easy to read and write, making it ideal for beginners.
- **Rich Standard Library:** A comprehensive set of built-in functions and libraries.
- **Cross-Platform:** Runs on Windows, macOS, and Linux.
- **Efficiency:** Designed for performance and low memory usage.
- **Extensibility:** Easily extendable with modules and libraries.

## Installation

To build and install the `cbudgie` interpreter, follow these steps:

1. **Clone the Repository**:
    ```sh
    git clone https://github.com/Hugodonotexit/CBudgie
    cd CBudgie
    ```

2. **Create a Build Directory**:
    ```sh
    mkdir build
    cd build
    ```

3. **Run CMake**:
    ```sh
    cmake ..
    ```

4. **Build the Project**:
    ```sh
    make
    ```

5. **Install the Executable**:
    ```sh
    sudo make install .
    ```

After these steps, the `cbudgie` executable will be installed to the system-wide location (usually `/usr/local/bin`), and you can run it from anywhere without the `./` prefix.

### Notes

- Ensure you have CMake installed. You can install it via your package manager:
    - **Ubuntu / Debian**:
        ```sh
        sudo apt-get install cmake
        ```
    - **Fedora**:
        ```sh
        sudo dnf install cmake
        ```
    - **macOS** (using Homebrew):
        ```sh
        brew install cmake
        ```

- If you encounter any permission issues during installation, you might need to use `sudo` for the install step.
- Make sure you have the necessary build tools and dependencies installed on your system. For example, on Ubuntu, you might need:

    ```sh
    sudo apt-get install build-essential
    ```

This setup allows you to compile and install the `cbudgie` interpreter, enabling you to use it without needing to specify the relative path.

## Grammar

[Refer to Documents](./docs/)

## Contributing

Contributions are welcome! If you have a useful snippet or improvement, feel free to fork the repository and submit a pull request.

   - Fork the repository.
   - Create a new branch for your feature: git checkout -b feature-name.
   - Commit your changes: git commit -m 'Add some feature'.
   - Push to the branch: git push origin feature-name.
   - Open a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact

For any questions or suggestions, please open an issue on the GitHub repository.