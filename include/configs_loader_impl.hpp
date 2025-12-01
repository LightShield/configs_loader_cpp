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
#include "cli/config_applier.hpp"
#include "help/help_generator.hpp"
#include "serialization/preset_deserializer.hpp"
#include "serialization/serializer_factory.hpp"
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
    
    const ParsedArguments args = CliArgumentParser::parse(argc, argv);
    
    if (args.preset_path.has_value()) {
        load_preset_file(args.preset_path.value());
    }
    
    ConfigApplier<ConfigsType> applier(configs);
    applier.apply_flags(args.flags);
    
    if (args.has_help) {
        std::cout << generate_help(argv[0], args.help_filter) << std::endl;
        std::exit(0);
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
    auto serializer = SerializerFactory<ConfigsType>::create(format);
    return serializer->serialize(configs, only_changes);
}

template<typename ConfigsType>
std::string ConfigsLoader<ConfigsType>::generate_help(const std::string& program_name, const std::string& filter) const {
    HelpGenerator<ConfigsType> generator(configs, help_config);
    return generator.generate(program_name, filter);
}


template<typename ConfigsType>
void ConfigsLoader<ConfigsType>::load_preset_file(const std::string& path) {
    auto deserializer = create_preset_deserializer(path);
    deserializer->parse_file(path);
    
    ConfigApplier<ConfigsType> applier(configs);
    applier.apply_deserializer(*deserializer);
}
