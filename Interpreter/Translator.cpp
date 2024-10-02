#include "Translator.h"

Translator::Translator(std::filesystem::path newFilePath,
                       std::filesystem::path destination)
    : filePath(newFilePath) {
  std::vector<std::vector<Token>> tokenized_code;
  std::vector<std::string> bytecode;
  Lexer lexer(filePath, tokenized_code);

  lexer.run();

  translate(tokenized_code, bytecode);

  writeToFile(bytecode, destination);
}

Translator::Translator(std::filesystem::path newFilePath) {
  std::filesystem::path destination = newFilePath;
  destination.replace_extension(".bbg");
  Translator(newFilePath, destination);
}

void Translator::translate(std::vector<std::vector<Token>>& tokenized_code,
                           std::vector<std::string>& bytecode) {
  std::deque<std::unordered_map<std::string, int>> variableMapByString;
  std::deque<std::unordered_map<int, std::string>> variableMapByInt;
  std::unordered_map<std::string, int> functionMap;
  std::unordered_map<std::string, int> undefinedFunctionMap;
  int translationCount = 0;
  std::stack<int> scopeCounts;
  std::stack<std::pair<int, int>> tempIfJump;
  std::stack<std::pair<int, std::vector<std::string>>> tempForJump;
  std::stack<std::pair<int, int>> loopPointer;
  std::vector<Token>::iterator linker;

  int commaCount = 0;

  variableMapByString.emplace_back();
  variableMapByInt.emplace_back();
  for (auto tokens : tokenized_code) {
    // Go through each token in the line and translate it to bytecode
    int start = bytecode.size();
    for (auto it = tokens.begin(); it < tokens.end(); ++it) {
      switch (it->tokenType) {
        case TokenType::NUM:
          if (linker->tokenType == TokenType::VARIABLE) {
            std::string opcode = "STORE ";
            int offset = -1;
            if (linker != tokens.begin()) {
              if ((linker - 1)->tokenType == TokenType::EQUAL) {
                bytecode.insert(bytecode.end() - 2, "NUM");
                break;
              } else if ((linker - 1)->tokenType == TokenType::R_SQBACKET) {
                if (std::distance(tokens.begin(), linker) >= 3 && (linker - 3)->tokenType == TokenType::L_SQBACKET && (linker - 2)->tokenType == TokenType::NUM_CONST) {
                  offset = std::stoi((linker - 2)->code);
                } else opcode = "STORE_SLOW ";
              }
            }
            int lookup = 0;
            auto variable = variableMapByString.back().find(linker->code);
            if (variable == variableMapByString.back().end())
              variable = variableMapByString.front().find(linker->code),
              lookup = 1;
            if (variable == variableMapByString.front().end())
              throw std::runtime_error("Invaild usage of numeric()!!");

            bytecode.push_back("NUM");
            bytecode.push_back(opcode + std::to_string(variable->second) +
                              (offset==-1 ? " " : (" " + std::to_string(offset) + " ")) + std::to_string(lookup));
          } else if (linker->tokenType == TokenType::READ) {
            bytecode.push_back("NUM");
          } else
            throw std::runtime_error("Invaild usage of numeric()!!");
          break;
        case TokenType::LINKER:
          linker = it - 1;
          break;
        case TokenType::TO_NUM:
          if (linker->tokenType == TokenType::VARIABLE) {
            std::string opcode = "STORE ";
            int offset = -1;
            if (linker != tokens.begin()) {
              if ((linker - 1)->tokenType == TokenType::EQUAL) {
                bytecode.insert(bytecode.end() - 2, "TO_NUM");
                break;
              } else if ((linker - 1)->tokenType == TokenType::R_SQBACKET) {
                if (std::distance(tokens.begin(), linker) >= 3 && (linker - 3)->tokenType == TokenType::L_SQBACKET && (linker - 2)->tokenType == TokenType::NUM_CONST) {
                  offset = std::stoi((linker - 2)->code);
                } else opcode = "STORE_SLOW ";
              }
            }
            int lookup = 0;
            auto variable = variableMapByString.back().find(linker->code);
            if (variable == variableMapByString.back().end())
              variable = variableMapByString.front().find(linker->code),
              lookup = 1;
            if (variable == variableMapByString.front().end())
              throw std::runtime_error("Invaild usage of toNum()!!");

            bytecode.push_back("TO_NUM");
            bytecode.push_back(opcode + std::to_string(variable->second) +
                              (offset==-1 ? " " : (" " + std::to_string(offset) + " ")) + std::to_string(lookup));
          } else if (linker->tokenType == TokenType::READ) {
            bytecode.push_back("TO_NUM");
          } else
            throw std::runtime_error("Invaild usage of toNum()!!");
          break;
        case TokenType::TO_STRING:
          if (linker->tokenType == TokenType::VARIABLE) {
            std::string opcode = "STORE ";
            int offset = -1;
            if (linker != tokens.begin()) {
              if ((linker - 1)->tokenType == TokenType::EQUAL) {
                bytecode.insert(bytecode.end() - 2, "TO_STRING");
                break;
              } else if ((linker - 1)->tokenType == TokenType::R_SQBACKET) {
                if (std::distance(tokens.begin(), linker) >= 3 && (linker - 3)->tokenType == TokenType::L_SQBACKET && (linker - 2)->tokenType == TokenType::NUM_CONST) {
                  offset = std::stoi((linker - 2)->code);
                } else opcode = "STORE_SLOW ";
              }
            }
            int lookup = 0;
            auto variable = variableMapByString.back().find(linker->code);
            if (variable == variableMapByString.back().end())
              variable = variableMapByString.front().find(linker->code),
              lookup = 1;
            if (variable == variableMapByString.front().end())
              throw std::runtime_error("Invaild usage of toString()!!");

            bytecode.push_back("TO_STRING");
            bytecode.push_back(opcode + std::to_string(variable->second) +
                              (offset==-1 ? " " : (" " + std::to_string(offset) + " ")) + std::to_string(lookup));
          } else if (linker->tokenType == TokenType::READ) {
            bytecode.push_back("TO_STRING");
          } else
            throw std::runtime_error("Invaild usage of toString()!!");
          break;
        case TokenType::TO_BOOL:
          if (linker->tokenType == TokenType::VARIABLE) {
            std::string opcode = "STORE ";
            int offset = -1;
            if (linker != tokens.begin()) {
              if ((linker - 1)->tokenType == TokenType::EQUAL) {
                bytecode.insert(bytecode.end() - 2, "TO_BOOL");
                break;
              } else if ((linker - 1)->tokenType == TokenType::R_SQBACKET) {
                if (std::distance(tokens.begin(), linker) >= 3 && (linker - 3)->tokenType == TokenType::L_SQBACKET && (linker - 2)->tokenType == TokenType::NUM_CONST) {
                  offset = std::stoi((linker - 2)->code);
                } else opcode = "STORE_SLOW ";
              }
            }
            int lookup = 0;
            auto variable = variableMapByString.back().find(linker->code);
            if (variable == variableMapByString.back().end())
              variable = variableMapByString.front().find(linker->code),
              lookup = 1;
            if (variable == variableMapByString.front().end())
              throw std::runtime_error("Invaild usage of toBool()!!");

            bytecode.push_back("TO_BOOL");
            bytecode.push_back(opcode + std::to_string(variable->second) +
                              (offset==-1 ? " " : (" " + std::to_string(offset) + " ")) + std::to_string(lookup));
          } else if (linker->tokenType == TokenType::READ) {
            bytecode.push_back("TO_BOOL");
          } else
            throw std::runtime_error("Invaild usage of toBool()!!");
          break;
        case TokenType::WHILE:
          bytecode.push_back("NOT");
          translationCount = bytecode.size() - 1;
          loopPointer.push(std::make_pair<int, int>(
              (int)scopeCounts.size(),
              (int)translationCount - std::distance(tokens.begin(), it)));
          bytecode.push_back("IF_JUMP ");
          tempIfJump.push(std::make_pair<int, int>((int)scopeCounts.size(),
                                                   (int)translationCount + 1));
          break;
        case TokenType::COLON:
          bytecode.push_back("COLON");
          break;
        case TokenType::FOR: {
          auto rit_bytecode =
              std::find(bytecode.rbegin(), bytecode.rend(), "COLON");
          auto it_bytecode = rit_bytecode.base() - 1;
          *it_bytecode = "LESS_THAN";
          std::vector<std::string> subvector(++it_bytecode, bytecode.end());
          bytecode.erase(it_bytecode, bytecode.end());

          rit_bytecode = std::find(rit_bytecode, bytecode.rend(), "COLON");
          it_bytecode = rit_bytecode.base() - 2;
          std::string copiedCommand = *it_bytecode;
          std::string copiedCommand1 = copiedCommand;
          copiedCommand.replace(0, 5, "LOAD");
          *(++it_bytecode) = copiedCommand;

          loopPointer.push(std::make_pair<int, int>(
              (int)scopeCounts.size(),
              (int)std::distance(bytecode.begin(), it_bytecode)));

          subvector.push_back(copiedCommand);
          subvector.push_back("ADD");
          subvector.push_back(copiedCommand1);

          bytecode.push_back("NOT");
          translationCount = bytecode.size() - 1;

          bytecode.push_back("IF_JUMP ");
          tempIfJump.push(std::make_pair<int, int>((int)scopeCounts.size(),
                                                   (int)translationCount + 1));
          tempForJump.emplace((int)scopeCounts.size(), subvector);
        } break;
        case TokenType::BREAK:
          bytecode.push_back("ENDSCOPE");
          bytecode.push_back("POP");
          if (scopeCounts.top() != variableMapByString.back().size()) {
            for (int i = scopeCounts.top(); i < variableMapByInt.back().size();
                 i++) {
              variableMapByString.back().erase(
                  variableMapByInt.back().find(i)->second);
              variableMapByInt.back().erase(i);
            }
          }
          bytecode.push_back("JUMP ");
          translationCount = bytecode.size() - 1;
          tempIfJump.push(std::make_pair<int, int>((int)tempIfJump.top().first,
                                                   (int)translationCount));
          break;
        case TokenType::ELSE:
          bytecode.push_back("NOT");
          bytecode.push_back("IF_JUMP ");
          translationCount = bytecode.size() - 1;
          tempIfJump.push(std::make_pair<int, int>((int)scopeCounts.size(),
                                                   (int)translationCount));
          break;
        case TokenType::IF:
          bytecode.push_back("NOT");
          bytecode.push_back("IF_JUMP ");
          translationCount = bytecode.size() - 1;
          tempIfJump.push(std::make_pair<int, int>((int)scopeCounts.size(),
                                                   (int)translationCount));
          break;
        case TokenType::CONTINUE:
          bytecode.push_back("ENDSCOPE");
          bytecode.push_back("POP");
          if (scopeCounts.top() != variableMapByString.back().size()) {
            for (int i = scopeCounts.top(); i < variableMapByInt.back().size();
                 i++) {
              variableMapByString.back().erase(
                  variableMapByInt.back().find(i)->second);
              variableMapByInt.back().erase(i);
            }
          }
          bytecode.push_back("JUMP " +
                             std::to_string(loopPointer.top().second));
          break;
        case TokenType::DEF: {
          while (it != tokens.end() && it->tokenType != TokenType::FUNCTION) {
            it++;
          }
          if (it == tokens.end())
            throw std::invalid_argument("Missing function name!!!");
          variableMapByString.emplace_back();
          variableMapByInt.emplace_back();
          auto function = functionMap.find(it->code);
          translationCount = bytecode.size() - 1;
          if (function == functionMap.end())
            functionMap.emplace(it->code, translationCount + 2);
          else if (function->second == -1) {
            function->second = translationCount + 1;
            int temp = undefinedFunctionMap.find(function->first)->second;
            bytecode[temp] += (std::to_string(function->second));
          }
        } break;
        case TokenType::L_SBACKET:
          bytecode.push_back("NEWSCOPE");
          scopeCounts.push(variableMapByString.back().size());
          break;
        case TokenType::R_SBACKET: {
          bytecode.push_back("ENDSCOPE");
          if (scopeCounts.top() != variableMapByString.back().size()) {
            for (int i = scopeCounts.top(); i < variableMapByInt.back().size();
                 i++) {
              variableMapByString.back().erase(
                  variableMapByInt.back().find(i)->second);
              variableMapByInt.back().erase(i);
            }
          }

          scopeCounts.pop();
          if (loopPointer.size() > 0 &&
              loopPointer.top().first == scopeCounts.size()) {
            bytecode.push_back("JUMP " +
                               std::to_string(loopPointer.top().second));
            loopPointer.pop();
          }

          translationCount = bytecode.size() - 1;
          if (tempIfJump.size() > 0 &&
              tempIfJump.top().first == scopeCounts.size()) {
            if (tempForJump.size() > 0 &&
                tempForJump.top().first == scopeCounts.size()) {
              bytecode.insert(bytecode.end() - 1,
                              tempForJump.top().second.begin(),
                              tempForJump.top().second.end());
              tempForJump.pop();
            }
            translationCount = bytecode.size() - 1;
            int temp = tempIfJump.top().first;
            bytecode[tempIfJump.top().second] +=
                std::to_string(translationCount + 1);
            tempIfJump.pop();
            if (tempIfJump.size() > 0 && temp == tempIfJump.top().first) {
              bytecode[tempIfJump.top().second] +=
                  std::to_string(translationCount + temp + 1);
              tempIfJump.pop();
            }
          }

        } break;
        case TokenType::RETURN:
          if (tokens.size() == 1) {
            bytecode.push_back("RETURN 0");
          } else {
            bytecode.push_back("RETURN 1");
          }
          variableMapByString.pop_back();
          variableMapByInt.pop_back();

          break;
        case TokenType::FUNCTION: {
          bytecode.insert(bytecode.begin() + start, "POP_ALL");
          auto function = functionMap.find(it->code);
          translationCount = bytecode.size() - 1;
          if (function == functionMap.end()) {
            functionMap.emplace(it->code, -1);
            bytecode.push_back("CALL ");
            undefinedFunctionMap.emplace(it->code, bytecode.size() - 1);
          } else {
            bytecode.push_back("CALL " + std::to_string(function->second));
          }
        } break;
        case TokenType::VARIABLE: {
          int lookup = 0;
          auto variable = variableMapByString.back().find(it->code);
          if (it != tokens.begin() &&
              (it - 1)->tokenType == TokenType::R_SQBACKET) {
                if (std::distance(tokens.begin(), it) >= 3 && (it - 3)->tokenType == TokenType::L_SQBACKET && (it - 2)->tokenType == TokenType::NUM_CONST)
                {
                  bytecode.pop_back();
                  if (variable == variableMapByString.back().end()) {
                    variable = variableMapByString.front().find(it->code);
                    if (variable == variableMapByString.front().end()) {
                      variableMapByString.back().emplace(
                          it->code, variableMapByString.back().size());
                      variableMapByInt.back().emplace(variableMapByInt.back().size(),
                                                      it->code);
                      variable = variableMapByString.back().find(it->code);
                      bytecode.push_back("LOAD " +
                                        std::to_string(variable->second) + " " +
                                        (it - 2)->code + " " +
                                        std::to_string(lookup));
                      break;
                    }
                    lookup = 1;
                  }
                  bytecode.push_back("LOAD " + std::to_string(variable->second) + " " + 
                                    (it - 2)->code + " " +
                                    std::to_string(lookup));
                } else {
                  if (variable == variableMapByString.back().end()) {
                    variable = variableMapByString.front().find(it->code);
                    if (variable == variableMapByString.front().end()) {
                      variableMapByString.back().emplace(
                          it->code, variableMapByString.back().size());
                      variableMapByInt.back().emplace(variableMapByInt.back().size(),
                                                      it->code);
                      variable = variableMapByString.back().find(it->code);
                      bytecode.push_back("LOAD_SLOW " +
                                        std::to_string(variable->second) + " " +
                                        std::to_string(lookup));
                      break;
                    }
                    lookup = 1;
                  }
                  bytecode.push_back("LOAD_SLOW " + std::to_string(variable->second) +
                                    " " + std::to_string(lookup));
                }
          } else {
            if (variable == variableMapByString.back().end()) {
              variable = variableMapByString.front().find(it->code);
              if (variable == variableMapByString.front().end()) {
                variableMapByString.back().emplace(
                    it->code, variableMapByString.back().size());
                variableMapByInt.back().emplace(variableMapByInt.back().size(),
                                                it->code);
                variable = variableMapByString.back().find(it->code);
                variableMapByString.back().find(it->code);
                bytecode.push_back("LOAD " + std::to_string(variable->second) + " 0 " + std::to_string(lookup));
                break;
              }
              lookup = 1;
            }
            bytecode.push_back("LOAD " + std::to_string(variable->second) + " 0 " +  std::to_string(lookup));
          }

        } break;
        case TokenType::TRUE:
          bytecode.push_back("LOAD_BOOLCONST 1");
          break;
        case TokenType::FALSE:
          bytecode.push_back("LOAD_BOOLCONST 0");
          break;
        case TokenType::NUM_CONST:
          bytecode.push_back("LOAD_NUMCONST " + it->code);
          break;
        case TokenType::WORD_CONST:
          bytecode.push_back("LOAD_WORDCONST " + it->code);
          break;
        case TokenType::COMMA:
          commaCount++;
          break;
        case TokenType::L_SQBACKET:
          commaCount = 0;
          break;
        case TokenType::EQUAL: {
          it++;
          int lookup = 0;
          if (it->tokenType == TokenType::VARIABLE) {
            auto variable = variableMapByString.back().find(it->code);
            if ((it - 2)->tokenType == TokenType::R_SQBACKET) {
              if (commaCount == 0) {
                auto variable = variableMapByString.back().find(it->code);
                if (variable == variableMapByString.back().end()) {
                  variable = variableMapByString.front().find(it->code);
                  if (variable == variableMapByString.front().end()) {
                    variableMapByString.back().emplace(
                        it->code, variableMapByString.back().size());
                    variableMapByInt.back().emplace(
                        variableMapByInt.back().size(), it->code);
                    variable = variableMapByString.back().find(it->code);
                    bytecode.push_back("STORE_SLOW " +
                                       std::to_string(variable->second) + " " +
                                       std::to_string(lookup));

                    break;
                  }
                  lookup = 1;
                }
                bytecode.push_back("STORE_SLOW " +
                                   std::to_string(variable->second) + " " +
                                   std::to_string(lookup));
              } else {
                if (variable == variableMapByString.back().end()) {
                  variable = variableMapByString.front().find(it->code);
                  if (variable == variableMapByString.front().end()) {
                    variableMapByString.back().emplace(
                        it->code, variableMapByString.back().size());
                    variableMapByInt.back().emplace(
                        variableMapByInt.back().size(), it->code);
                    variable = variableMapByString.back().find(it->code);
                    bytecode.push_back("STORE_ALL " +
                                       std::to_string(variable->second) + " " +
                                       std::to_string(commaCount + 1) + " " +
                                       std::to_string(lookup));
                    break;
                  }
                  lookup = 1;
                }
                bytecode.push_back("STORE_ALL " +
                                   std::to_string(variable->second) + " " +
                                   std::to_string(commaCount + 1) + " " +
                                   std::to_string(lookup));
              }
            } else {
              if (variable == variableMapByString.back().end()) {
                variable = variableMapByString.front().find(it->code);
                if (variable == variableMapByString.front().end()) {
                  variableMapByString.back().emplace(
                      it->code, variableMapByString.back().size());
                  variableMapByInt.back().emplace(
                      variableMapByInt.back().size(), it->code);
                  variable = variableMapByString.back().find(it->code);
                  bytecode.push_back("STORE " +
                                     std::to_string(variable->second) + " 0 " +
                                     std::to_string(lookup));
                  break;
                }
                lookup = 1;
              }
              bytecode.push_back("STORE " + std::to_string(variable->second) +
                                 " 0 " + std::to_string(lookup));
            }
          } else
            throw std::invalid_argument("Invalid argument: " + it->code);
        } break;
        case TokenType::PLUS:
          bytecode.push_back("ADD");
          break;
        case TokenType::MINUS:
          bytecode.push_back("SUB");
          break;
        case TokenType::TIMES:
          bytecode.push_back("MUL");
          break;
        case TokenType::DIVIDE:
          bytecode.push_back("DIV");
          break;
        case TokenType::MODE:
          bytecode.push_back("MOD");
          break;
        case TokenType::POW:
          bytecode.push_back("POW");
          break;
        case TokenType::AND:
          bytecode.push_back("AND");
          break;
        case TokenType::NOT:
          bytecode.push_back("NOT");
          break;
        case TokenType::XOR:
          bytecode.push_back("XOR");
          break;
        case TokenType::OR:
          bytecode.push_back("OR");
          break;
        case TokenType::GREATER:
          bytecode.push_back("GREATER_THAN");
          break;
        case TokenType::SMALLER:
          bytecode.push_back("LESS_THAN");
          break;
        case TokenType::E_GREATER:
          bytecode.push_back("GREATER_THAN_EQUAL");
          break;
        case TokenType::E_SMALLER:
          bytecode.push_back("LESS_THAN_EQUAL");
          break;
        case TokenType::EQUIVALENT:
          bytecode.push_back("EQUAL");
          break;
        case TokenType::NOT_EQUIVALENT:
          bytecode.push_back("NOT_EQUAL");
          break;
        case TokenType::READ:
          bytecode.push_back("READ");
          break;
        case TokenType::PRINT:
          bytecode.push_back("PRINT");
          break;
        default:
          break;
      }
      translationCount = bytecode.size() - 1;
    }
  }

  // main function is the entry point and required for a program to run
  auto variable = functionMap.find("main");
  if (variable == functionMap.end())
    throw std::invalid_argument("Missing main() function!");
}

void Translator::writeToFile(std::vector<std::string>& bytecode,
                             const std::filesystem::path& outputPath) {
  std::ofstream outFile(outputPath);
  for (auto line : bytecode) {
    outFile << line << std::endl;
  }

  outFile.close();
}
