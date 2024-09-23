#include "Interpreter.h"

Interpreter::Interpreter(std::filesystem::path file){
    
    std::filesystem::path directory = file.parent_path() / ".__bgCache__";
    std::filesystem::path source = file;
    source.replace_extension(".bbg");
    std::filesystem::path destination = directory / source.filename();

    try {
        // Create directory if it doesn't exist
        if (!std::filesystem::exists(directory)) {
            std::filesystem::create_directory(directory);
            //////std::cout << "Directory created: " << directory << std::endl;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    Translator translator(file,destination);
    
    VM vm(destination);

}