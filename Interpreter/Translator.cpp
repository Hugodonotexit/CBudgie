#include "Translator.h"

void Translator::writeToFile(std::vector<std::string>& bytecode,
                             const std::filesystem::path& outputPath) {
  std::ofstream outFile(outputPath);
  for (auto line : bytecode) {
    outFile << line << std::endl;
  }

  outFile.close();
}

void Translator::translate(std::vector<std::string>& bytecode, AST* tree) {

}

Translator::Translator(AST* tree, std::filesystem::path destination) {
    std::vector<std::string> bytecode;

    translate(bytecode, tree);

    writeToFile(bytecode, destination);
}
