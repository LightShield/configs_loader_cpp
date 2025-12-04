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
    if (init(argc, argv) != 0) {
        std::exit(1);
    }
}

template<typename ConfigsType>
int ConfigsLoader<ConfigsType>::init(int argc, char* argv[]) {
    if (help_format.program_name == "program" && argc > 0) {
        help_format.program_name = argv[0];
    }
    
    ConfigValidator<ConfigsType> validator(configs);
    validator.validate_reserved_flags();
    if (validator.has_errors()) {
        std::cerr << validator.get_error_report();
        return 1;
    }
    
    const ParsedArguments args = CliArgumentParser::parse(argc, argv);
    
    ConfigApplier<ConfigsType> applier(configs);
    
    if (args.preset_path.has_value()) {
        auto deserializer = create_preset_deserializer(args.preset_path.value());
        deserializer->parse_file(args.preset_path.value());
        applier.apply_from_preset(*deserializer);
    }
    
    applier.apply_from_cli(args.flags);
    
    if (applier.has_errors()) {
        std::cerr << "Configuration application failed with " << applier.get_errors().size() << " error(s):\n\n";
        for (const auto& error : applier.get_errors()) {
            std::cerr << "  • Validation failed for flag '" << error.flag << "'";
            if (!error.description.empty()) {
                std::cerr << " (" << error.description << ")";
            }
            std::cerr << ": value = " << error.value << "\n";
        }
        return 1;
    }
    
    if (args.has_help) {
        std::cout << generate_help(args.help_filter) << std::endl;
        std::exit(0);
    }
    
    validator.validate_required_fields();
    if (validator.has_errors()) {
        std::cerr << validator.get_error_report();
        return 1;
    }
    
    m_initialized = true;
    return 0;
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
std::string ConfigsLoader<ConfigsType>::generate_help(const std::string& filter, 
                                                      std::optional<std::reference_wrapper<const HelpFormat>> format) const {
    const HelpFormat& fmt = format.value_or(std::cref(help_format));
    HelpGenerator<ConfigsType> generator(configs, fmt);
    return generator.generate(filter);
}


