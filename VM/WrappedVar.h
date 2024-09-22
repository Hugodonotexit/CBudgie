#pragma once
#include <cmath>
#include <string>
#include <algorithm>
#include <cmath>

using Var = std::variant<long double, bool, std::string>;

class WrappedVar {
public:
    Var value;

    WrappedVar(const Var& v) : value(v) {}

    WrappedVar operator+(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, long double> && !std::is_same_v<U, std::string>) {
                return lhs + (long double)rhs;
            } else if constexpr (std::is_same_v<T, bool> && !std::is_same_v<U, std::string>) {
                return lhs || rhs;
            } else if constexpr (std::is_same_v<T, std::string> && std::is_same_v<U, bool>) {
                return lhs + (rhs ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string> && std::is_same_v<U, long double>) {
                return lhs + std::to_string(rhs);
            } else if constexpr (std::is_same_v<T, std::string> && std::is_same_v<U, std::string>) {
                return lhs + rhs;
            } else {
                throw std::invalid_argument("Incompatible types for addition");
            }
        }, value, other.value);
    }

    WrappedVar operator-(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, long double> && std::is_same_v<U, long double>) {
                return lhs - rhs;
            } else {
                throw std::invalid_argument("Incompatible types for subtraction");
            }
        }, value, other.value);
    }

    WrappedVar operator*(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, long double> && std::is_same_v<U, long double>) {
                return lhs * rhs;
            } else {
                throw std::invalid_argument("Incompatible types for multiplication ");
            }
        }, value, other.value);
    }

    WrappedVar operator/(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, long double> && std::is_same_v<U, long double>) {
                return lhs / rhs;
            } else {
                throw std::invalid_argument("Incompatible types for division");
            }
        }, value, other.value);
    }
    
    WrappedVar operator%(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, long double> && std::is_same_v<U, long double>) {
                return std::fmod(lhs, rhs);
            } else {
                throw std::invalid_argument("Incompatible types for modulor");
            }
        }, value, other.value);
    }

    WrappedVar pow(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, long double> && std::is_same_v<U, long double>) {
                return std::pow(lhs,rhs);
            } else {
                throw std::invalid_argument("Incompatible types for power");
            }
        }, value, other.value);
    }

    WrappedVar operator!=(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, long double> && std::is_same_v<U, long double>) {
                return lhs != rhs;
            } else if constexpr (std::is_same_v<T, bool> && std::is_same_v<U, bool>) {
                return lhs != rhs;
            } else if constexpr (std::is_same_v<T, std::string> && std::is_same_v<U, std::string>) {
                return lhs != rhs;
            } else {
                return false;
            }
        }, value, other.value);
    }

    WrappedVar operator==(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, long double> && std::is_same_v<U, long double>) {
                return lhs == rhs;
            } else if constexpr (std::is_same_v<T, bool> && std::is_same_v<U, bool>) {
                return lhs == rhs;
            } else if constexpr (std::is_same_v<T, std::string> && std::is_same_v<U, std::string>) {
                return lhs == rhs;
            } else {
                return false;
            }
        }, value, other.value);
    }

    WrappedVar operator>=(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, long double> && std::is_same_v<U, long double>) {
                return lhs >= rhs;
            } else if constexpr (std::is_same_v<T, bool> && std::is_same_v<U, bool>) {
                return lhs >= rhs;
            } else if constexpr (std::is_same_v<T, std::string> && std::is_same_v<U, std::string>) {
                return lhs >= rhs;
            } else {
                return false;
            }
        }, value, other.value);
    }

    WrappedVar operator<=(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, long double> && std::is_same_v<U, long double>) {
                return lhs <= rhs;
            } else if constexpr (std::is_same_v<T, bool> && std::is_same_v<U, bool>) {
                return lhs <= rhs;
            } else if constexpr (std::is_same_v<T, std::string> && std::is_same_v<U, std::string>) {
                return lhs <= rhs;
            } else {
                return false;
            }
        }, value, other.value);
    }

    WrappedVar operator>(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, long double> && std::is_same_v<U, long double>) {
                return lhs > rhs;
            } else if constexpr (std::is_same_v<T, bool> && std::is_same_v<U, bool>) {
                return lhs > rhs;
            } else if constexpr (std::is_same_v<T, std::string> && std::is_same_v<U, std::string>) {
                return lhs > rhs;
            } else {
                return false;
            }
        }, value, other.value);
    }

    WrappedVar operator<(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, long double> && std::is_same_v<U, long double>) {
                return lhs < rhs;
            } else if constexpr (std::is_same_v<T, bool> && std::is_same_v<U, bool>) {
                return lhs < rhs;
            } else if constexpr (std::is_same_v<T, std::string> && std::is_same_v<U, std::string>) {
                return lhs < rhs;
            } else {
                return false;
            }
        }, value, other.value);
    }

    WrappedVar operator!() const {
        return std::visit([](auto&& lhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            if constexpr (std::is_same_v<T, long double>) {
                return !lhs;
            } else if constexpr (std::is_same_v<T, bool>) {
                return !lhs;
            } else if constexpr (std::is_same_v<T, std::string>) {
                std::string reversedString = lhs;
                std::reverse(reversedString.begin(), reversedString.end());
                return reversedString;
            } 
        }, value);
    }
    
    WrappedVar operator^(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
        using T = std::decay_t<decltype(lhs)>;
        using U = std::decay_t<decltype(rhs)>;
        
        if constexpr (std::is_same_v<T, long double> && std::is_same_v<U, long double>) {
            return (long double)(static_cast<long long>(lhs) ^ static_cast<long long>(rhs));
        } else if constexpr (std::is_same_v<T, bool> && std::is_same_v<U, bool>) {
            return lhs != rhs; // Logical XOR for bools
        } else {
            return true;
        }
    }, value, other.value);
    }
    
    WrappedVar operator||(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, long double> && std::is_same_v<U, long double>) {
                return lhs || rhs;
            } else if constexpr (std::is_same_v<T, bool> && std::is_same_v<U, bool>) {
                return lhs || rhs;
            } else {
                return true;
            }
        }, value, other.value);
    }

    WrappedVar operator&&(const WrappedVar& other) const {
        return std::visit([](auto&& lhs, auto&& rhs) -> Var {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, long double> && std::is_same_v<U, long double>) {
                return lhs && rhs;
            } else if constexpr (std::is_same_v<T, bool> && std::is_same_v<U, bool>) {
                return lhs && rhs;
            } else {
                return false;
            }
        }, value, other.value);
    }

    friend std::ostream& operator<<(std::ostream& os, const WrappedVar& wrappedVar) {
        std::visit([&os](auto&& val) {
            os << val;
        }, wrappedVar.value);
        return os;
    }

    explicit operator bool() const {
        return std::visit([](auto&& val) -> bool {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, long double>) {
                return val > 0.0;
            } else if constexpr (std::is_same_v<T, bool>) {
                return val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return val != "";
            }
            return false;
        }, value);
    }
};