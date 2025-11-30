#pragma once

#include "config.hpp"
#include <optional>
#include <string>
#include <type_traits>

namespace {

template<typename T>
void load_field(PresetDeserializer& deserializer, Config<T>& field);

template<typename T>
void load_field(PresetDeserializer& deserializer, ConfigGroup<T>& group);

template<typename T>
void load_field(PresetDeserializer& deserializer, Config<T>& field) {
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

template<typename T>
void load_field(PresetDeserializer& deserializer, ConfigGroup<T>& group) {
    auto fields = group.get_fields();
    std::apply([&](auto&... field) {
        ((load_field(deserializer, field)), ...);
    }, fields);
}

}

template<typename ConfigsType>
void PresetDeserializer::load_into(ConfigsType& configs) {
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        ((load_field(*this, field)), ...);
    }, fields);
}
