#include <string>
#include "cli.h"
#include "Lexer.h"
#include "Parser.h"
#include "Translator.h"
#include "VirtualMachine.h"
int main(int argc, char* argv[]) {
    std::string option = argv[1];
    Lexer Lexer;
    Parser Parser;
    Translator Translator;
    VirtualMachine VirtualMachine;
    if (argc < 2) {
        cli cli(std::move(Lexer), std::move(Parser), std::move(Translator), std::move(VirtualMachine), std::move(argc), argv);
        return 0;
    }

}