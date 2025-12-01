#pragma once

#include "config.hpp"
#include "serialization/preset_deserializer.hpp"
#include <algorithm>
#include <type_traits>

template<typename ConfigsType>
void ConfigApplier<ConfigsType>::apply_flags(const std::unordered_map<std::string, std::string>& flags) {
    for (const auto& [flag, value] : flags) {
        auto fields = m_configs.get_fields();
        std::apply([&](auto&... field) {
            (try_set_field(field, flag, value) || ...);
        }, fields);
    }
}

template<typename ConfigsType>
void ConfigApplier<ConfigsType>::apply_deserializer(PresetDeserializer& deserializer) {
    auto fields = m_configs.get_fields();
    std::apply([&](auto&... field) {
        ((load_field(deserializer, field)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
void ConfigApplier<ConfigsType>::load_field(PresetDeserializer& deserializer, Config<T>& field) {
    if (field.flags.empty()) return;
    
    for (const auto& flag : field.flags) {
        std::string key = flag;
        if (key.starts_with("--")) {
            key = key.substr(2);
        } else if (key.starts_with("-")) {
            key = key.substr(1);
        }
        
        std::optional<T> value;
        if constexpr (std::is_same_v<T, std::string>) {
            value = deserializer.get_string(key);
        } else if constexpr (std::is_same_v<T, int>) {
            value = deserializer.get_int(key);
        } else if constexpr (std::is_same_v<T, bool>) {
            value = deserializer.get_bool(key);
        } else if constexpr (std::is_same_v<T, double>) {
            value = deserializer.get_double(key);
        }
        
        if (value.has_value()) {
            field.set_value(*value);
            return;
        }
    }
}

template<typename ConfigsType>
template<typename T>
void ConfigApplier<ConfigsType>::load_field(PresetDeserializer& deserializer, ConfigGroup<T>& group) {
    auto fields = group.get_fields();
    std::apply([&](auto&... field) {
        ((load_field(deserializer, field)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
bool ConfigApplier<ConfigsType>::try_set_field(Config<T>& field, const std::string& flag, const std::string& value) {
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
bool ConfigApplier<ConfigsType>::try_set_field(ConfigGroup<T>& group, const std::string& flag, const std::string& value) {
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
