#pragma once

#include "config.hpp"
#include <type_traits>

template<typename ConfigsType>
std::optional<std::string> CliArgumentParser<ConfigsType>::extract_preset_path(int argc, char* argv[]) {
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
void CliArgumentParser<ConfigsType>::parse(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.empty() || arg[0] != '-') {
            continue;
        }

        if (arg == "--preset" || arg == "-p") {
            ++i;
            continue;
        }

        std::string value;
        bool has_value = false;

        size_t equals_pos = arg.find('=');
        if (equals_pos != std::string::npos) {
            std::string flag = arg.substr(0, equals_pos);
            if (flag == "--preset" || flag == "-p") {
                continue;
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
            set_config_value(arg, value);
        }
    }
}

template<typename ConfigsType>
void CliArgumentParser<ConfigsType>::set_config_value(const std::string& flag, const std::string& value) {
    auto fields = m_configs.get_fields();
    std::apply([&](auto&... field) {
        (try_set_field(field, flag, value) || ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
bool CliArgumentParser<ConfigsType>::try_set_field(Config<T>& field, const std::string& flag, const std::string& value) {
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
template<typename T>
bool CliArgumentParser<ConfigsType>::try_set_field(ConfigGroup<T>& group, const std::string& flag, const std::string& value) {
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
        return (try_set_field(field, nested_flag, value) || ...);
    }, fields);
}
