#include <vector>

#include "depen/var.h"
#include "lexer/lexer.h"
#include "lexer/readFile.h"
#include "parser/parser.h"

using namespace std;

int main(int argc, char* argv[]) {
  // clock_t start_time, end_time;
  // double cpu_time_used;

  // start_time = clock();
  ReadFile readFile(argc, argv);
  vector<string> input = readFile.getInput();
  vector<vector<Token>> tokens;
  Lexer lexer(input);
  try {
    tokens = lexer.tokenize();
    /*Test only
    for (const auto& token : tokens) {
      for (const auto& token_line : token)
      {
        cout << "Token: " << token_line.value
        << " Type: " << static_cast<int>(token_line.type) << endl;
      }
      cout << endl;
    }
    */
  } catch (const runtime_error& e) {
    cerr << "Error: " << e.what() << endl;
  }
  try {
    Parser_main parser(tokens);
  } catch (const runtime_error& e) {
    cerr << "Error: " << e.what() << endl;
  }

  // end_time = clock();
  // cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

  // printf("Time taken: %f seconds\n", cpu_time_used);

  return 0;
}