#include "cli.h"

void cli::runline()
{   
    while (true) {
        std::cout << ">>";
        std::string cmd;
        std::getline(std::cin,cmd);
        while (isExpected("\\", std::to_string(cmd[cmd.length()-1]))) {
            std::cout << "  -";
            std::string temp;
            std::getline(std::cin,temp);
            cmd = cmd + temp; 
        }
        std::istringstream iss(cmd);
        std::vector<std::string> token;

        for (std::string word; iss >> word;) token.push_back(word);
        this->VM.run(this->Trans.run(this->Pars.run(this->Lex.run(token))));
    }
}

void cli::run_argv()
{
    int i = 2;
    std::vector<std::string> cmd_string;
    std::string bracket;
    std::string word = this->argv[i++];
    if (!(isExpected("\"", word) || isExpected("\'", word))) {
        std::cerr << "Expecting \" or \'" << std::endl;
        throw std::exception();
    }
    bracket = word[0];
    while (true) {
        if (isExpected(bracket, word) && i == this->argc-1) {
            break;
        } else if (i == this->argc-1)
        {
            std::cerr << "Expecting \" or \'" << std::endl;
            throw std::exception();
        }
        cmd_string.push_back(word);
        i++;
    }
    this->VM.run(this->Trans.run(this->Pars.run(this->Lex.run(cmd_string))));
    runline();
}

bool cli::isExpected(std::string expected, std::string actual)
{
    if (expected == actual) return true;
    return false;
}

cli::cli(Lexer Lexer, Parser Parser, Translator Translator, VirtualMachine VirtualMachine, int argc, char *argv[])
{
    this->Lex = Lexer;
    this->Pars = Parser;
    this->Trans = Translator;
    this->VM = VirtualMachine;
    this->argc = argc;
    for (size_t i = 0; i < argc; i++)
    {
        this->argv.push_back(argv[i]);
    }
    
    if (argc < 2) {
        runline();
    } else {
        run_argv();
    }
}

cli::~cli()
{
}
