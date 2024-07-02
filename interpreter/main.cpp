#include <vector>
#include <utility>
#include "depen/var.h"
#include "lexer/lexer.h"
#include "lexer/readFile.h"
#include "parser/parser.h"

using namespace std;

int main(int argc, char* argv[]) {
  //auto start = std::chrono::high_resolution_clock::now();

  ReadFile readFile(argc, argv);
  try {
    Parser_main parser(readFile.tokenize(),readFile.getrange());
  } catch (const exception& e) {
    cerr << "Error: " << e.what() << endl;
  }

  //auto end = std::chrono::high_resolution_clock::now();
  //std::chrono::duration<double> elapsed = end - start;
  //std::cout << "Elapsed time: " << std::fixed << std::setprecision(4) << elapsed.count() << " seconds" << std::endl;
  return 0;
}