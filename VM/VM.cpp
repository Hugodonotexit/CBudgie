#include "VM.h"

VM::VM(const std::filesystem::path& filePath) {
  readbytecode file;
  instructions = file.read_file(filePath);
  int instructionPointer = 0;
  std::stack<int> scopeCounts;
  stack.emplace();
  bool run = true;

  while (instructionPointer < instructions.size() && run) {
    auto& instruction = instructions[instructionPointer];
    switch (instruction.opcode) {
      case Opcode::POP:
        stack.top().pop();
        break;
      case Opcode::ENDSCOPE:
        stack.pop();
        if (scopeCounts.top() != var.size()) {
          var.erase(var.begin() + (scopeCounts.top() - 1), var.end());
        }
        scopeCounts.pop();
        break;
      case Opcode::NEWSCOPE:
        scopeCounts.push(var.size());
        stack.emplace();
        break;
      case Opcode::NOP:
        continue;
      case Opcode::TO_STRING:
        if (std::holds_alternative<long double>(stack.top().top().value)) {
          std::string temp =
              std::to_string(std::get<long double>(stack.top().top().value));
          stack.top().pop();
          stack.top().push(WrappedVar(temp));
        } else if (std::holds_alternative<bool>(stack.top().top().value)) {
          std::string temp =
              std::get<bool>(stack.top().top().value) ? "True" : "False";
          stack.top().pop();
          stack.top().push(WrappedVar(temp));
        }

        break;
      case Opcode::TO_BOOL:

        if (std::holds_alternative<std::string>(stack.top().top().value)) {
          std::string str = std::get<std::string>(stack.top().top().value);
          if (str == "F" || str == "0" || str == "False") {
            stack.top().pop();
            stack.top().push(WrappedVar(false));
          } else {
            stack.top().pop();
            stack.top().push(WrappedVar(true));
          }

        } else if (std::holds_alternative<long double>(
                       stack.top().top().value)) {
          bool temp = std::get<long double>(stack.top().top().value) > 0;
          stack.top().pop();
          stack.top().push(WrappedVar(temp));
        }

        break;
      case Opcode::TO_NUM:
        if (std::holds_alternative<std::string>(stack.top().top().value)) {
          try {
            size_t pos;
            std::string str = std::get<std::string>(stack.top().top().value);
            long double temp = std::stold(str, &pos);

            // Check if the entire string was converted
            if (pos != str.length()) {
              throw std::invalid_argument("Extra characters after number");
            }
            stack.top().pop();
            stack.top().push(WrappedVar(temp));
          } catch (const std::invalid_argument&) {
            std::cerr << "Invalid argument: not a valid long double"
                      << std::endl;
          } catch (const std::out_of_range&) {
            std::cerr << "Out of range: number too large" << std::endl;
          }
        } else if (std::holds_alternative<long double>(
                       stack.top().top().value)) {
          long double temp = std::get<bool>(stack.top().top().value);
          stack.top().pop();
          stack.top().push(WrappedVar(temp));
        }
        break;
      case Opcode::LOAD:
        stack.top().push(var[instruction.index][instruction.offset]);
        break;
      case Opcode::LOAD_ALL:
        for (int i = instruction.offset - 1; i >= 0; i++) {
          stack.top().push(var[instruction.index][i]);
        }
        break;
      case Opcode::STORE:
        if (instruction.index >= var.size()) {
          std::vector<WrappedVar> a = {WrappedVar(stack.top().top())};
          var.push_back(a);
        } else {
          if (instruction.offset < var[instruction.index].size()) {
            var[instruction.index][instruction.offset] = stack.top().top();
          } else {
            var[instruction.index].push_back(stack.top().top());
          }
        }
        stack.top().pop();
        break;
      case Opcode::STORE_ALL:
        if (instruction.index < var.size()) {
          for (int i = 0; i < instruction.offset; i++) {
            if (i < var[instruction.index].size()) {
              var[instruction.index][i] = stack.top().top();
              stack.top().pop();
            } else {
              var[instruction.index].push_back(stack.top().top());
              stack.top().pop();
            }
          }
        } else {
          std::vector<WrappedVar> temp;
          for (int i = 0; i < instruction.offset; i++) {
            temp.push_back(stack.top().top());
            stack.top().pop();
          }
          var.push_back(temp);
        }
        stack.top().pop();
        break;
      case Opcode::LOAD_BOOLCONST:
        stack.top().push(WrappedVar(instruction.boolValue));
        break;
      case Opcode::LOAD_NUMCONST:
        stack.top().push(WrappedVar(instruction.numValue));
        break;
      case Opcode::LOAD_WORDCONST:
        stack.top().push(WrappedVar(instruction.stringValue));
        break;
      case Opcode::AND: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b && a);
      } break;
      case Opcode::OR: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b || a);
      } break;
      case Opcode::XOR: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b ^ a);
      } break;
      case Opcode::NOT: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        stack.top().push(!a);
      } break;
      case Opcode::GREATER_THAN_EQUAL: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b >= a);
      } break;
      case Opcode::GREATER_THAN: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b > a);
      } break;
      case Opcode::LESS_THAN_EQUAL: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b <= a);
      } break;
      case Opcode::LESS_THAN: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b < a);
      } break;
      case Opcode::EQUAL: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b == a);
      } break;
      case Opcode::NOT_EQUAL: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b != a);
      } break;
      case Opcode::READ: {
        std::string words;
        std::cin >> words;
        stack.top().push(WrappedVar(words));
      } break;
      case Opcode::PRINT: {
        WrappedVar a(stack.top().top());
        stack.top().pop();
        std::cout << a << std::endl;
      } break;
      case Opcode::MOD: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b % a);
      } break;
      case Opcode::POW: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b.pow(a));
      } break;
      case Opcode::DIV: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b / a);
      } break;
      case Opcode::MUL: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b * a);
      } break;
      case Opcode::SUB: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b - a);
      } break;
      case Opcode::ADD: {
        WrappedVar a = stack.top().top();
        stack.top().pop();
        WrappedVar b = stack.top().top();
        stack.top().pop();
        stack.top().push(b + a);
      } break;
      case Opcode::RETURN:
        instructionPointer = functionPointers.top();
        functionPointers.pop();
        if (scopeCounts.top() != var.size()) {
          var.erase(var.begin() + (scopeCounts.top() - 1), var.end());
        }
        scopeCounts.pop();
        
        if (stack.size() <= 2) {
          run = false;
          continue;
        }

        if (stack.top().empty() || !instruction.boolValue) {
          stack.pop();
        } else {
          WrappedVar temp = stack.top().top();
          stack.pop();
          stack.top().push(temp);
        }

        break;
      case Opcode::CALL:
        functionPointers.push(instructionPointer);
        instructionPointer = instruction.index;
        scopeCounts.push(var.size());
        {
          std::vector<std::vector<WrappedVar>> temp;
          for (int i = 0; i < instruction.offset; i++) {
            std::vector<WrappedVar> tmp;
            tmp.push_back(stack.top().top());
            temp.push_back(tmp);
            stack.top().pop();
          }
          var.insert(var.end(), temp.begin(), temp.end());
          stack.emplace();
        }
        continue;
      case Opcode::IF_JUMP:
        if (stack.empty() || stack.top().empty()) {
          throw std::runtime_error("Stack is empty");
        }
        if (stack.top().top()) {
          instructionPointer = instruction.index;
          continue;
        }
        stack.top().pop();

        break;
      case Opcode::JUMP:
        instructionPointer = instruction.index;
        continue;
    }
    instructionPointer++;
  }
  if (!stack.empty()) {
    if (!stack.top().empty()) {;
      if (stack.top().top() != WrappedVar(0.0)) throw std::runtime_error("!!!Program exit with non-zero value!!!");
    }
  }
}