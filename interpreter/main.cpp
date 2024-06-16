#include <vector>

#include "lexer.h"
#include "var.h"
#include "readFile.h"
#include "parser.h"

using namespace std;

int main(int argc, char* argv[]) {
  clock_t start_time, end_time;
  double cpu_time_used;
  
  start_time = clock();

  ReadFile readFile(argc, argv);
  vector<string> input = readFile.getInput();

  Lexer lexer(input);
  try {
    vector<vector<Token>> tokens = lexer.tokenize();
    Parser parser(tokens);
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

  end_time = clock();
  cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

  printf("Time taken: %f seconds\n", cpu_time_used);

  return 0;
}