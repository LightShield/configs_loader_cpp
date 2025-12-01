#pragma once

#include <string>
#include <unordered_map>

// Forward declaration
class PresetDeserializer;

template<typename ConfigsType>
class ConfigApplier {
    ConfigsType& m_configs;

public:
    explicit ConfigApplier(ConfigsType& configs) : m_configs(configs) {}

    void apply_flags(const std::unordered_map<std::string, std::string>& flags);
    void apply_deserializer(PresetDeserializer& deserializer);

private:
    template<typename T> bool try_set_field(Config<T>& field, const std::string& flag, const std::string& value);
    template<typename T> bool try_set_field(ConfigGroup<T>& group, const std::string& flag, const std::string& value);
    template<typename T> void load_field(PresetDeserializer& deserializer, Config<T>& field);
    template<typename T> void load_field(PresetDeserializer& deserializer, ConfigGroup<T>& group);
};

#include "config_applier_impl.hpp"
