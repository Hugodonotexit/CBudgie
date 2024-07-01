#include <vector>
#include <utility>
#include "depen/var.h"
#include "lexer/lexer.h"
#include "lexer/readFile.h"
#include "parser/parser.h"

using namespace std;

int main(int argc, char* argv[]) {
  ReadFile readFile(argc, argv);
  Lexer lexer(readFile.getInput());
  try {
    Parser_main parser(lexer.tokenize());
  } catch (const runtime_error& e) {
    cerr << "Error: " << e.what() << endl;
  }
  return 0;
}