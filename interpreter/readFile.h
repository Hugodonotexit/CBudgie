#ifndef READFILE_H
#define READFILE_H
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

using namespace std;
class ReadFile
{
private:
    vector<string> input;
public:
    ReadFile(int argc, char* argv[]);
    void read_File(string file_path);
    vector<string> &getInput();
    ~ReadFile() {};
};

ReadFile::ReadFile(int argc, char* argv[])
{
      // Check if the file path is provided as a command-line argument
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <file_path>" << endl;
        abort();
    }

    string file_path = argv[1];
    read_File(file_path);
}

void ReadFile::read_File(string file_path)
{
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

    while (getline(file, line)) {
      input.push_back(line);
    }

    file.close();
}

vector<string> &ReadFile::getInput() {return input;}

#endif