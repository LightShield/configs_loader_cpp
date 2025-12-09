#pragma once

#include <string>
#include <type_traits>

namespace lightshield::config {

namespace ansi {
    constexpr const char* RESET = "\033[0m";
    constexpr const char* BOLD = "\033[1m";
    constexpr const char* RED = "\033[31m";
    constexpr const char* GREEN = "\033[32m";
    constexpr const char* YELLOW = "\033[33m";
    constexpr const char* BLUE = "\033[34m";
    constexpr const char* MAGENTA = "\033[35m";
    constexpr const char* CYAN = "\033[36m";
    constexpr const char* GRAY = "\033[90m";
}

inline std::string colorize(const std::string& text, const char* const color, const bool use_colors) {
    if (!use_colors) return text;
    return std::string(color) + text + ansi::RESET;
}

template<typename T>
constexpr const char* get_type_name() {
    if constexpr (std::is_same_v<T, std::string>) return "string";
    else if constexpr (std::is_same_v<T, int>) return "int";
    else if constexpr (std::is_same_v<T, bool>) return "bool";
    else if constexpr (std::is_same_v<T, double>) return "double";
    else if constexpr (std::is_enum_v<T>) return "enum";
    else return "unknown";
}

}  // namespace lightshield::config
