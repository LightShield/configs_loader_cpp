#pragma once

#include <optional>
#include <string>

template<typename ConfigsType>
class CliArgumentParser {
    ConfigsType& m_configs;

public:
    explicit CliArgumentParser(ConfigsType& configs) : m_configs(configs) {}

    void parse(int argc, char* argv[]);
    std::optional<std::string> extract_preset_path(int argc, char* argv[]);

private:
    void set_config_value(const std::string& flag, const std::string& value);
    
    template<typename T> bool try_set_field(Config<T>& field, const std::string& flag, const std::string& value);
    template<typename T> bool try_set_field(ConfigGroup<T>& group, const std::string& flag, const std::string& value);
};

#include "cli_argument_parser_impl.hpp"
