#include "Interpreter.h"

Interpreter::Interpreter(std::filesystem::path file, bool translate_only){
    std::filesystem::path path = file.parent_path() / "__bgCache__" / file.filename();
    path.replace_extension("bbg");
    run(file, path, translate_only);
}

Interpreter::Interpreter(std::filesystem::path file, std::filesystem::path path){
    path = path / file.filename();
    path.replace_extension("bbg");
    run(file, path, true);
}

void Interpreter::run(std::filesystem::path file, std::filesystem::path path, bool translate_only) {
    try {
        // Create directory if it doesn't exist
        if (!std::filesystem::exists(path.parent_path())) {
            std::filesystem::create_directory(path.parent_path());
            //////std::cout << "Directory created: " << path << std::endl;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::vector<std::string> lines = openfile(file);

    Lexer lexer(lines);
    std::vector<std::vector<Token>> tokenized = lexer.run();

    Parser parser(tokenized);
    AST* tree = parser.run();
    parser.printAST(tree);
    Translator translator(tree);
    std::vector<std::string> bytecode = translator.run();

    writeToFile(bytecode, path);
    
    if (!translate_only)
    {
        auto future = std::async(std::launch::async, [path]() {
            VirtualMachine vm(path);
            try {
                std::filesystem::remove_all(path.parent_path());
            } catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        });
    }

    delete tree;
}

std::vector<std::string> Interpreter::openfile(const std::filesystem::path& filePath) {
  std::ifstream file(filePath);

  // Attempt to open file
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + filePath.filename().string());
  }

  std::string strline;
  std::vector<std::string> lines;
  // Loop through each line of the file and convert to tokens
  while (std::getline(file, strline)) {
    lines.push_back(strline);
  }

  file.close();

  return lines;
}

void Interpreter::writeToFile(std::vector<std::string>& bytecode, const std::filesystem::path& outputPath) {
  std::ofstream outFile(outputPath);
  for (auto line : bytecode) {
    outFile << line << std::endl;
  }

  outFile.close();
}