#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace lightshield::config {


// Forward declaration
class PresetDeserializer;

struct ApplyError {
    std::string flag;
    std::string value;
    std::string description;
};

template<typename ConfigsType>
class ConfigApplier {
    ConfigsType& m_configs;
    std::vector<ApplyError> m_errors;

public:
    explicit ConfigApplier(ConfigsType& configs) : m_configs(configs) {}

    void apply_from_preset(PresetDeserializer& deserializer);
    void apply_from_cli(const std::unordered_map<std::string, std::string>& flags);
    
    bool has_errors() const { return !m_errors.empty(); }
    const std::vector<ApplyError>& get_errors() const { return m_errors; }

private:
    template<typename T> bool try_set_field(Config<T>& field, const std::string& flag, const std::string& value);
    template<typename T> bool try_set_field(ConfigGroup<T>& group, const std::string& flag, const std::string& value);
    template<typename T> void load_field(PresetDeserializer& deserializer, Config<T>& field);
    template<typename T> void load_field(PresetDeserializer& deserializer, ConfigGroup<T>& group);
};

}  // namespace lightshield::config

#include "config_applier_impl.hpp"
