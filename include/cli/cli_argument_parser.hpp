#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace lightshield::config {


struct ParsedArguments {
    std::unordered_map<std::string, std::string> flags;
    std::optional<std::string> preset_path;
    std::string help_filter;
    bool has_help = false;
    bool print_config = false;
    bool print_config_verbose = false;
};

class CliArgumentParser {
public:
    static ParsedArguments parse(int argc, char* argv[]);
};

}  // namespace lightshield::config

#include "cli_argument_parser_impl.hpp"
