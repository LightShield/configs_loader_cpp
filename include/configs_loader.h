#pragma once

#include "config_field.h"
#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <type_traits>
#include <vector>
#include <tuple>

#define REGISTER_CONFIG_FIELDS(...) \
    auto get_fields() { \
        return std::tie(__VA_ARGS__); \
    }

template<typename ConfigsType>
class ConfigsLoader {
public:
    ConfigsType configs;

    ConfigsLoader() = default;

    ConfigsLoader(int argc, char* argv[]) {
        parse_arguments(argc, argv);
    }

    void parse_arguments(int argc, char* argv[]) {
        if (argc > 1) {
            std::optional<std::string> preset_path = extract_preset_path(argc, argv);
            
            if (preset_path.has_value()) {
                load_preset_file(preset_path.value());
            }

            parse_cli_arguments(argc, argv);
        }
        
        validate_required_fields();
    }

private:
    std::optional<std::string> extract_preset_path(int argc, char* argv[]) {
        for (int i = 1; i < argc - 1; ++i) {
            std::string arg = argv[i];
            if (arg == "--preset" || arg == "-p") {
                return std::string(argv[i + 1]);
            }
        }
        return std::nullopt;
    }

    void load_preset_file([[maybe_unused]] const std::string& path) {
        // TODO: Implement JSON parsing
    }

    void parse_cli_arguments(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg.empty() || arg[0] != '-') {
                continue;
            }

            std::string value;
            bool has_value = false;

            size_t equals_pos = arg.find('=');
            if (equals_pos != std::string::npos) {
                value = arg.substr(equals_pos + 1);
                arg = arg.substr(0, equals_pos);
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

    template<typename T>
    bool try_set_field_value(ConfigField<T>& field, const std::string& flag, const std::string& value) {
        const auto& flags = field.flags();
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

    void try_set_config_value(const std::string& flag, const std::string& value) {
        auto fields = configs.get_fields();
        std::apply([&](auto&... field) {
            (try_set_field_value(field, flag, value) || ...);
        }, fields);
    }

    void validate_required_fields() {
        auto fields = configs.get_fields();
        std::apply([&](auto&... field) {
            ((validate_field(field)), ...);
        }, fields);
    }

    template<typename T>
    void validate_field(const ConfigField<T>& field) {
        if (field.is_required() && !field.is_set()) {
            throw std::runtime_error("Required config field not set");
        }
    }
};
