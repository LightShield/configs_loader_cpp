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

#include "cli/cli_argument_parser.hpp"
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
        CliArgumentParser<ConfigsType> parser(configs);
        const std::optional<std::string> preset_path = parser.extract_preset_path(argc, argv);
        
        if (preset_path.has_value()) {
            load_preset_file(preset_path.value());
        }

        parser.parse(argc, argv);
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
void ConfigsLoader<ConfigsType>::load_preset_file(const std::string& path) {
    auto deserializer = create_preset_deserializer(path);
    deserializer->parse_file(path);
    deserializer->load_into(configs);
}
