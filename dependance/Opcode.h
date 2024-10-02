#pragma once
#include <string>
#include <unordered_map>
enum class Opcode {
  
  LOAD,
  STORE,
  LOAD_SLOW,
  STORE_SLOW,

  STORE_ALL,

  LOAD_NUMCONST,

  LOAD_WORDCONST,

  LOAD_BOOLCONST,

  RETURN,

  IF_JUMP,
  JUMP,
  CALL,

  ADD,
  SUB,
  MUL,
  DIV,
  MOD,
  POW,
  POP,
  POP_ALL,

  TO_STRING,
  TO_BOOL,
  TO_NUM,

  LESS_THAN,
  GREATER_THAN,
  EQUAL,
  NOT_EQUAL,
  LESS_THAN_EQUAL,
  GREATER_THAN_EQUAL,

  NUM,

  AND,
  OR,
  XOR,
  NOT,

  READ,
  PRINT,
  
  NEWSCOPE,
  ENDSCOPE,
  NOP
};

static const std::unordered_map<std::string, Opcode> stringToOpcode = {
    {"CALL", Opcode::CALL},
    {"LOAD", Opcode::LOAD},
    {"STORE", Opcode::STORE},
    {"STORE_ALL", Opcode::STORE_ALL},
    {"STORE_SLOW", Opcode::STORE_SLOW},
    {"LOAD_SLOW", Opcode::LOAD_SLOW},
    {"LOAD_NUMCONST", Opcode::LOAD_NUMCONST},
    {"LOAD_WORDCONST", Opcode::LOAD_WORDCONST},
    {"LOAD_BOOLCONST", Opcode::LOAD_BOOLCONST},
    {"RETURN", Opcode::RETURN},
    {"IF_JUMP", Opcode::IF_JUMP},
    {"JUMP", Opcode::JUMP},
    {"ADD", Opcode::ADD},
    {"SUB", Opcode::SUB},
    {"MUL", Opcode::MUL},
    {"DIV", Opcode::DIV},
    {"POW", Opcode::POW},
    {"MOD", Opcode::MOD},
    {"POP", Opcode::POP},
    {"NUM", Opcode::NUM},
    {"POP_ALL", Opcode::POP_ALL},
    {"TO_STRING", Opcode::TO_STRING},
    {"TO_BOOL", Opcode::TO_BOOL},
    {"TO_NUM", Opcode::TO_NUM},
    {"LESS_THAN", Opcode::LESS_THAN},
    {"GREATER_THAN", Opcode::GREATER_THAN},
    {"EQUAL", Opcode::EQUAL},
    {"NOT_EQUAL", Opcode::NOT_EQUAL},
    {"LESS_THAN_EQUAL", Opcode::LESS_THAN_EQUAL},
    {"GREATER_THAN_EQUAL", Opcode::GREATER_THAN_EQUAL},
    {"AND", Opcode::AND},
    {"OR", Opcode::OR},
    {"XOR", Opcode::XOR},
    {"NOT", Opcode::NOT},
    {"READ", Opcode::READ},
    {"PRINT", Opcode::PRINT},
    {"NEWSCOPE", Opcode::NEWSCOPE},
    {"ENDSCOPE",Opcode::ENDSCOPE},
    {"", Opcode::NOP},
    {"#", Opcode::NOP}
};
