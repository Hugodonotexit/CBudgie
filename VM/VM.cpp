#include "VM.h"

VM::VM(const std::filesystem::path& filePath) {
  readbytecode file;
  instructions = file.read_file(filePath);
  int instructionPointer = 0;
  std::stack<int> scopeCounts;
  stack.emplace();
  bool run = true;
  var.emplace_back();
  while (instructionPointer < instructions.size() && run) {
    Instruction& instruction = instructions[instructionPointer];
    switch (instruction.opcode) {
      case Opcode::NUM:
      {
        WrappedVar instruction = stack.top().top();
        stack.top().pop();
        if (instruction == WrappedVar("max"))
        {
          stack.top().push(WrappedVar(std::numeric_limits<long double>::max()));
        } else if (instruction == WrappedVar("min")) {
          stack.top().push(WrappedVar(std::numeric_limits<long double>::min()));
        } else if (instruction == WrappedVar("abs")) {
          WrappedVar value = stack.top().top();
          stack.top().pop();
          stack.top().push(WrappedVar(budgieMath::abs(value.toNum())));
        } else if (std::holds_alternative<long double>(instruction.value)) {
          WrappedVar instruction2 = stack.top().top();
          stack.top().pop();
          WrappedVar value = stack.top().top();
          stack.top().pop();
          if (instruction2 == WrappedVar("round")) {
            value = WrappedVar(budgieMath::round(value.toNum(), instruction.toNum()));
          } else if (instruction2 == WrappedVar("floor")) {
            value = WrappedVar(budgieMath::floor(value.toNum(), instruction.toNum()));
          } else if (instruction2 == WrappedVar("ceil")) {
            value = WrappedVar(budgieMath::ceil(value.toNum(), instruction.toNum()));
          } 
          stack.top().push(value);
        } else throw std::invalid_argument("Illegal use of numeric()!");
      } 
        break;
      case Opcode::POP_ALL:
        if (stack.empty()) break;
        while (!stack.top().empty())
        {
          stack.top().pop();
        }
        break;
      case Opcode::POP:
        stack.top().pop();
        break;
      case Opcode::ENDSCOPE:
        stack.pop();
        if (scopeCounts.top() != var.size()) {
          var.back().erase(var.back().begin() + scopeCounts.top(), var.back().end());
        }
        scopeCounts.pop();
        break;
      case Opcode::NEWSCOPE:
        scopeCounts.push(var.back().size());
        stack.emplace();
        break;
      case Opcode::NOP:
        break;
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
          stack.top().top() = WrappedVar(stack.top().top().toNum());
        break;
      case Opcode::LOAD_SLOW:
      {
        int index = stack.top().top().toNum();
        stack.top().pop();
        if (instruction.offset == 0) {
          stack.top().push(var.back()[instruction.index][index]);
        } else {
          stack.top().push(var.front()[instruction.index][index]);
        }
      }
        break;
      case Opcode::STORE_SLOW:
      {
        int index = stack.top().top().toNum();
        stack.top().pop();
        if (instruction.offset == 0) {
            if (instruction.index >= var.back().size()) {
              throw std::out_of_range("Attempt to store an element to an list");
            } else {
              if (index >= var.back()[instruction.index].size()) {
                var.back()[instruction.index].push_back(stack.top().top());
              } else {
                var.back()[instruction.index][index] = stack.top().top();
              }
            }
          } else {
            if (instruction.index >= var.front().size()) {
              throw std::out_of_range("Attempt to store an element to an list");
            } else {
              if (index >= var.front()[instruction.index].empty()) {
                var.front()[instruction.index].push_back(stack.top().top());
              } else {
                var.front()[instruction.index][index] = stack.top().top();
              }
            }
          }
          
          stack.top().pop();
        }
        break;
      case Opcode::LOAD:
      if (instruction.offset == 0) {
        stack.top().push(var.back()[instruction.index][0]);
      } else {
        stack.top().push(var.front()[instruction.index][0]);
      }
        break;
      case Opcode::STORE:
        if (instruction.offset == 0) {
          if (instruction.index >= var.back().size()) {
            std::vector<WrappedVar> a = {WrappedVar(stack.top().top())};
            var.back().push_back(a);
          } else {
            if (var.back()[instruction.index].empty()) {
              var.back()[instruction.index].push_back(stack.top().top());
            } else {
              var.back()[instruction.index][0] = stack.top().top();
            }
          }
        } else {
          if (instruction.index >= var.front().size()) {
            std::vector<WrappedVar> a = {WrappedVar(stack.top().top())};
            var.front().push_back(a);
          } else {
            if (var.front()[instruction.index].empty()) {
              var.front()[instruction.index].push_back(stack.top().top());
            } else {
              var.front()[instruction.index][0] = stack.top().top();
            }
          }
        }
        
        stack.top().pop();
        break;
      case Opcode::STORE_ALL:
      {
        auto& selectedVar = instruction.state == 0 ? var.back() : var.front();
        if (instruction.index < selectedVar.size()) {
          for (int i=0; i < instruction.offset;i++) {
            if (i < selectedVar[instruction.index].size()) {
              selectedVar[instruction.index][i] = stack.top().top();
            } else {
              selectedVar[instruction.index].push_back(stack.top().top());
            }
            stack.top().pop();
          }
        } else {
          std::vector<WrappedVar> temp;
          for (int i = 0; i < instruction.offset; i++) {
            temp.push_back(stack.top().top());
            stack.top().pop();
          }
          selectedVar.push_back(temp);
        }
      }
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
        WrappedVar a(stack.top().top());
        stack.top().pop();
        std::cout << a << std::endl;
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
        
        var.pop_back();
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
        scopeCounts.push(var.back().size());
        {
          std::vector<std::vector<WrappedVar>> temp;
          while (!stack.top().empty()) {
            std::vector<WrappedVar> tmp;
            tmp.push_back(stack.top().top());
            temp.push_back(tmp);
            stack.top().pop();
          }
          std::reverse(temp.begin(), temp.end());
          var.emplace_back(temp);
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