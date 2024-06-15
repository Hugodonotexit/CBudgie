#include <vector>

#include "lexer.h"
#include "var.h"
#include "readFile.h"

using namespace std;

int main(int argc, char* argv[]) {
  ReadFile readFile(argc, argv);
  vector<string> input = readFile.getInput();

  Lexer lexer(input);
  try {
    vector<vector<Token>> tokens = lexer.tokenize();
    //Parser parser(tokens);
    for (const auto& token : tokens) {
      for (const auto& token_line : token)
      {
        cout << "Token: " << token_line.value
        << " Type: " << static_cast<int>(token_line.type) << endl;
      }
      cout << endl << endl;
      
    }
  } catch (const runtime_error& e) {
    cerr << "Error: " << e.what() << endl;
  }

  return 0;
}