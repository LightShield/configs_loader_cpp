#pragma once

#include <optional>
#include <string>
#include <unordered_map>

struct ParsedArguments {
    std::unordered_map<std::string, std::string> flags;
    std::optional<std::string> preset_path;
    std::optional<std::string> help_filter;
    bool has_help = false;
};

class CliArgumentParser {
public:
    static ParsedArguments parse(int argc, char* argv[]);
};

#include "cli_argument_parser_impl.hpp"
