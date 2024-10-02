#include "readbytecode.h"
Instruction readbytecode::read_line(const std::string& line) {
  std::istringstream stream(line);
  std::string opcodeStr;
  stream >> opcodeStr;
  Opcode opcode = parseOpcode(opcodeStr);
  if (opcode >= Opcode::ADD) return Instruction(opcode);

  if (opcode == Opcode::LOAD_NUMCONST) {
    return Instruction(opcode, parseNumOperand(stream));
  }
  if (opcode == Opcode::LOAD_WORDCONST) {
    return Instruction(opcode, parseStringOperand(stream));
  }

  std::array<int, 3> operands = parseIntOperands(stream);

  if (opcode == Opcode::LOAD_BOOLCONST || opcode == Opcode::RETURN) {
    return Instruction(opcode, (operands[0] > 0));
  }

  if (opcode <= Opcode::STORE_SLOW) {
    return Instruction(opcode, operands[0], operands[1]);
  }

  if (opcode == Opcode::STORE_ALL) {
    return Instruction(opcode, operands[0], operands[1], operands[2]);
  }

  if (opcode >= Opcode::IF_JUMP && opcode <= Opcode::CALL) {
    return Instruction(opcode, operands[0]);
  }

  throw std::runtime_error("Invalid line: " + line);
}

Opcode readbytecode::parseOpcode(const std::string& str) {
  auto it = stringToOpcode.find(str);
  if (it != stringToOpcode.end()) {
    return it->second;
  }
  throw std::runtime_error("Unknown opcode: " + str);
}

std::string readbytecode::parseStringOperand(std::istringstream& stream) {
    std::string operand;
    std::string token;
    while (stream >> token) {
        if (token.front() == '"') {
            std::string quotedString = token.substr(1); // Remove the starting quote
            while (quotedString.back() != '"' && stream >> token) {
                quotedString += " " + token;
            }
            if (quotedString.back() == '"') {
                quotedString.pop_back(); // Remove the ending quote
            }
            operand = quotedString;
            break;
        }
    }
    return operand;
}

long double readbytecode::parseNumOperand(std::istringstream& stream) {
    long double operand;
    stream >> operand;
  return operand;
}

std::array<int, 3> readbytecode::parseIntOperands(std::istringstream& stream) {
  std::array<int, 3> operands;
  int operand;
  int index = 0;
  while (stream >> operand) {
    operands[index++] = operand;
  }
  return operands;
}

std::vector<Instruction> readbytecode::read_file(const std::filesystem::path &filePath) {
  std::vector<Instruction> Instructions;

  std::ifstream file(filePath);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + filePath.filename().string());
  }
  std::string line;
  while (std::getline(file, line)) {
    Instruction instruction = read_line(line);
    if (instruction.opcode != Opcode::NOP) {
      if (instruction.opcode == Opcode::ENDSCOPE && Instructions.back().opcode == Opcode::NEWSCOPE)
      {
        Instructions.pop_back();
        continue;
      }
      Instructions.push_back(instruction);
    }
  }

  file.close();
  return Instructions;
}
