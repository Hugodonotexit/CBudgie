#include "Interpreter.h"

Interpreter::Interpreter(std::filesystem::path file) : Translator(file) {
    std::filesystem::path directory = file.parent_path() / ".__bgCache__";
    std::filesystem::path source = file.replace_extension(".bbg");
    std::filesystem::path destination = directory / source.filename();

    try {
        // Create directory if it doesn't exist
        if (!std::filesystem::exists(directory)) {
            std::filesystem::create_directory(directory);
            //////std::cout << "Directory created: " << directory << std::endl;
        }

        // Move the file
        std::filesystem::rename(source, destination);
        //////std::cout << "File moved to: " << destination << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    VM vm(destination);

    try {
        if (std::filesystem::exists(directory)) {
            std::filesystem::remove_all(directory);
            //////std::cout << "Directory and its contents deleted successfully." << std::endl;
        } else {
            //////std::cout << "Directory does not exist." << std::endl;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

}