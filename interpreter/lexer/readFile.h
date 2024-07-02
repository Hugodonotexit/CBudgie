#ifndef READFILE_H
#define READFILE_H
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include "lexer.h"

using namespace std;
class ReadFile : public Lexer {
public:
  ~ReadFile(){};
  inline ReadFile(int argc, char* argv[]) {
    // Check if the file path is provided as a command-line argument
    if (argc != 2) {
      cerr << "Usage: " << argv[0] << " <file_path>" << endl;
      abort();
    }
    for (size_t i = 1; i < argc; i++) {
      string file_path = argv[i];
      read_File(file_path);
    }
  }
  private:
  inline void read_File(string file_path) {
    filesystem::path path(file_path);
    string extension = path.extension().string();
    if (extension != ".bg") {
      cerr << "Error: Unsupported file " << file_path << endl;
      abort();
    }

    ifstream file(file_path);
    if (!file.is_open()) {
      cerr << "Error: Could not open file " << file_path << endl;
      abort();
    }
    string line;
    string name = path.filename().string();

    input.push_back("`" + name.substr(0, name.size() - 3));
    while (getline(file, line)) {
      input.push_back(line);
    }

    file.close();
  }
};

#endif