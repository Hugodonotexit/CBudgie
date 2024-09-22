#include <iostream>
#include <filesystem>
#include <string>
#include "../VM/VM.h"
#include "../Interpreter/Interpreter.h"
#include "../Interpreter/Translator.h"
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: cbudgie <file_path>     Run a cbudgie script\n"
                  << "Usage: cbudgie [options] <file_path>\n"
                  << "Options:\n"
                  << "  -h, --help                   Show this help message\n"
                  << "  -t, --translate <file.bg>    Output bytecode\n"
                  << "  -b, --bytecode <file.bbg>    Run a bytecode script\n";
        return 1;
    }

    std::string option = argv[1];
    std::filesystem::path file;

    if (option == "-h" || option == "--help") {
        std::cout << "Usage: cbudgie <file_path>     Run a cbudgie script\n"
                  << "Usage: cbudgie [options] <file_path>\n"
                  << "Options:\n"
                  << "  -h, --help                   Show this help message\n"
                  << "  -t, --translate <file.bg>    Output bytecode\n"
                  << "  -b, --bytecode <file.bbg>    Run a bytecode script\n";
    } else if (option == "-t" || option == "--translate") {
        if (argc < 3) {
            std::cerr << "Error: Missing file path for translation.\n";
            return 1;
        }
        file = argv[2];
        if (file.extension().string() != ".bg") {
            std::cerr << "Error: Expecting a cbudgie file (.bg).\n";
            return 1;
        }
        //////std::cout << "Translating " << file.filename().string() << " to bytecode.\n";
        Translator translator(file);

    } else if (option == "-b" || option == "--bytecode") {
        if (argc < 3) {
            std::cerr << "Error: Missing bytecode file path.\n";
            return 1;
        }
        file = argv[2];
        if (file.extension().string() != ".bbg") {
            std::cerr << "Error: Expecting a bytecode file (.bbg).\n";
            return 1;
        }
        //////std::cout << "Running bytecode script " << file.filename().string() << ".\n";
        VM vm(file);
    } else {
        file = argv[1];
        if (file.extension().string() != ".bg") {
            std::cerr << "Error: Expecting a cbudgie file (.bg).\n";
            return 1;
        }
        //////std::cout << "Running script " << file.filename().string() << ".\n";
        Interpreter interpreter(file);
    }

    return 0;
}