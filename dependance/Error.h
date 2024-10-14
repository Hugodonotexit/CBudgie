#include <exception>
#include <string>

namespace cbg {

class UnknowError : public std::exception {
public:
    explicit UnknowError(const std::string& message) : msg_(message) {}

    const char* what() const noexcept override {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

class VirtualMachineError : public UnknowError {};

class InterpreterError : public UnknowError {};

class ParserError : public InterpreterError {};

class LexerError : public InterpreterError {};

class TranslatorError : public InterpreterError {};

}