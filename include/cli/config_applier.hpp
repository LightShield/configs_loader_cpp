#pragma once

#include <string>
#include <unordered_map>

template<typename ConfigsType>
class ConfigApplier {
    ConfigsType& m_configs;

public:
    explicit ConfigApplier(ConfigsType& configs) : m_configs(configs) {}

    void apply(const std::unordered_map<std::string, std::string>& flags);

private:
    template<typename T> bool try_set_field(Config<T>& field, const std::string& flag, const std::string& value);
    template<typename T> bool try_set_field(ConfigGroup<T>& group, const std::string& flag, const std::string& value);
};

#include "config_applier_impl.hpp"
