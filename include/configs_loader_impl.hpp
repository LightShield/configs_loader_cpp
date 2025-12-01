#pragma once

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <vector>

#include "help/help_generator.hpp"
#include "serialization/cli_serializer.hpp"
#include "serialization/preset_deserializer.hpp"
#include "serialization/toml_serializer.hpp"
#include "validation/config_validator.hpp"

template<typename ConfigsType>
ConfigsLoader<ConfigsType>::ConfigsLoader(int argc, char* argv[]) {
    init(argc, argv);
}

template<typename ConfigsType>
void ConfigsLoader<ConfigsType>::init(int argc, char* argv[]) {
    ConfigValidator<ConfigsType> validator(configs);
    validator.validate_reserved_flags();
    if (validator.has_errors()) {
        throw std::runtime_error(validator.get_error_report());
    }
    
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            std::string filter;
            if (help_config.enable_interactive && i + 1 < argc && argv[i + 1][0] != '-') {
                filter = argv[i + 1];
            }
            std::cout << generate_help(argv[0], 80, filter) << std::endl;
            std::exit(0);
        }
    }
    
    if (argc > 1) {
        const std::optional<std::string> preset_path = extract_preset_path(argc, argv);
        
        if (preset_path.has_value()) {
            load_preset_file(preset_path.value());
        }

        parse_cli_arguments(argc, argv);
    }
    
    validator.validate_required_fields();
    if (validator.has_errors()) {
        throw std::runtime_error(validator.get_error_report());
    }
    
    m_initialized = true;
}

template<typename ConfigsType>
bool ConfigsLoader<ConfigsType>::is_initialized() const {
    return m_initialized;
}

template<typename ConfigsType>
std::string ConfigsLoader<ConfigsType>::dump_configs(SerializationFormat format, bool only_changes) const {
    switch (format) {
        case SerializationFormat::CLI: {
            CliSerializer<ConfigsType> serializer(configs, only_changes);
            return serializer.serialize();
        }
        case SerializationFormat::TOML: {
            TomlSerializer<ConfigsType> serializer(configs, only_changes);
            return serializer.serialize();
        }
    }
    return "";
}

template<typename ConfigsType>
std::string ConfigsLoader<ConfigsType>::generate_help(const std::string& program_name, size_t max_width, const std::string& filter) const {
    HelpGenerator<ConfigsType> generator(configs, help_config);
    return generator.generate(program_name, max_width, filter);
}

template<typename ConfigsType>
std::optional<std::string> ConfigsLoader<ConfigsType>::extract_preset_path(int argc, char* argv[]) {
    for (int i = 1; i < argc - 1; ++i) {
        const std::string arg = argv[i];
        if (arg == "--preset" || arg == "-p") {
            return std::string(argv[i + 1]);
        }
        
        const size_t equals_pos = arg.find('=');
        if (equals_pos != std::string::npos) {
            const std::string flag = arg.substr(0, equals_pos);
            if (flag == "--preset" || flag == "-p") {
                return arg.substr(equals_pos + 1);
            }
        }
    }
    return std::nullopt;
}

template<typename ConfigsType>
void ConfigsLoader<ConfigsType>::load_preset_file(const std::string& path) {
    auto deserializer = create_preset_deserializer(path);
    deserializer->parse_file(path);
    deserializer->load_into(configs);
}

template<typename ConfigsType>
void ConfigsLoader<ConfigsType>::parse_cli_arguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.empty() || arg[0] != '-') {
            continue;
        }

        // Skip preset flag - it's internal
        if (arg == "--preset" || arg == "-p") {
            ++i; // Skip the value too
            continue;
        }

        std::string value;
        bool has_value = false;

        size_t equals_pos = arg.find('=');
        if (equals_pos != std::string::npos) {
            std::string flag = arg.substr(0, equals_pos);
            if (flag == "--preset" || flag == "-p") {
                continue; // Skip preset
            }
            value = arg.substr(equals_pos + 1);
            arg = flag;
            has_value = true;
        } else if (i + 1 < argc && argv[i + 1][0] != '-') {
            value = argv[i + 1];
            has_value = true;
            ++i;
        }

        if (has_value) {
            try_set_config_value(arg, value);
        }
    }
}

template<typename ConfigsType>
template<typename T>
bool ConfigsLoader<ConfigsType>::try_set_field_value(Config<T>& field, const std::string& flag, const std::string& value) {
    const auto& flags = field.flags;
    if (std::find(flags.begin(), flags.end(), flag) == flags.end()) {
        return false;
    }

    T converted_value;
    if constexpr (std::is_same_v<T, std::string>) {
        converted_value = value;
    } else if constexpr (std::is_same_v<T, int>) {
        converted_value = std::stoi(value);
    } else if constexpr (std::is_same_v<T, bool>) {
        converted_value = (value == "true" || value == "1");
    } else if constexpr (std::is_same_v<T, double>) {
        converted_value = std::stod(value);
    } else {
        return false;
    }

    return field.set_value(converted_value);
}

template<typename ConfigsType>
void ConfigsLoader<ConfigsType>::try_set_config_value(const std::string& flag, const std::string& value) {
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        (try_set_field_value(field, flag, value) || ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
bool ConfigsLoader<ConfigsType>::try_set_field_value(ConfigGroup<T>& group, const std::string& flag, const std::string& value) {
    std::string flag_prefix;
    std::string flag_body;
    if (flag.starts_with("--")) {
        flag_prefix = "--";
        flag_body = flag.substr(2);
    } else if (flag.starts_with("-")) {
        flag_prefix = "-";
        flag_body = flag.substr(1);
    } else {
        flag_body = flag;
    }
    
    const std::string prefix_with_dot = group.get_name() + ".";
    if (flag_body.find(prefix_with_dot) != 0) {
        return false;
    }
    
    const std::string nested_flag_body = flag_body.substr(prefix_with_dot.length());
    const std::string nested_flag = flag_prefix + nested_flag_body;
    
    auto fields = group.get_fields();
    return std::apply([&](auto&... field) {
        return (try_set_field_value(field, nested_flag, value) || ...);
    }, fields);
}
