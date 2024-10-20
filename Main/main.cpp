#include <iostream>
#include <filesystem>
#include <string>

#include "../VirtualMachine/VirtualMachine.h"
#include "../Interpreter/Interpreter.h"
#include "../Interpreter/Parser.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 3
#define VERSION_PATCH 0
#define VERSION_PRE_RELEASE "alpha"


//#include <chrono>
int main(int argc, char* argv[]) {
    //auto start = std::chrono::high_resolution_clock::now();
    
    if (argc < 2) {
        std::cerr << "Usage: cbudgie <file_path>                Run a cbudgie script\n"
                  << "Usage: cbudgie [options] <file_path>\n"
                  << "Options:\n"
                  << "  -h, --help                              Show this help message\n"
                  << "  -t, --translate <file.bg> [destination] Output bytecode, destination default \"./\"\n"
                  << "  -b, --bytecode <file.bbg>               Run a bytecode script\n"
                  << "  -v, --version                           Output version\n";
        return 1;
    }

    std::string option = argv[1];
    std::filesystem::path file;

    if (option == "-h" || option == "--help") {
        std::cout << "Usage: cbudgie <file_path>                Run a cbudgie script\n"
                  << "Usage: cbudgie [options] <file_path>\n"
                  << "Options:\n"
                  << "  -h, --help                              Show this help message\n"
                  << "  -t, --translate <file.bg> [destination] Output bytecode, destination default \"./\"\n"
                  << "  -b, --bytecode <file.bbg>               Run a bytecode script\n"
                  << "  -v, --version                           Output version\n";
    } else if (option == "-v" || option == "--version") {
        std::cout << "cbudgie " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH;

        #if defined(VERSION_PRE_RELEASE)

            std::cout << "-" << VERSION_PRE_RELEASE;
        #endif

        std::cout << std::endl;
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
        std::filesystem::path path;
        if (argc > 3) {
            path = argv[3];
        } else {
            path = "./";
        }
        //////std::cout << "Translating " << file.filename().string() << " to bytecode.\n";
        Interpreter interpreter(file, path);

    } else if (option == "-b" || option == "--bytecode") {
        if (argc < 3) {
            std::cerr << "Error: Missing bytecode file path.\n";
            return 1;
        }
        file = argv[2];
        if (file.extension().string() != ".bbg") {
            std::cerr << "Error: Expecting a cbudgie bytecode file (.bbg).\n";
            return 1;
        }
        //////std::cout << "Running bytecode script " << file.filename().string() << ".\n";
        VirtualMachine vm(file);
    } else if (option == "crazy" || option == "Crazy") {
        std::ios::sync_with_stdio(false);
        for (unsigned long long i = 0; i < std::numeric_limits<unsigned long long>::max(); i++)
        {
            std::cout << "Crazy? I was crazy once. They locked me in a room. A rubber room. A rubber room with rats. And rats make me crazy.\n";
        }
        std::cout << "Bro no way you waited!!!\n";
    } else {
        file = argv[1];
        if (file.extension().string() != ".bg") {
            std::cerr << "Error: Expecting a cbudgie file (.bg).\n";
            return 1;
        }
        //////std::cout << "Running script " << file.filename().string() << ".\n";
        Interpreter interpreter(file);
    }

    //auto end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> duration = end - start;
    
    //std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    return 0;
}