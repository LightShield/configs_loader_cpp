#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace lightshield::config {

template<typename T>
struct Config {
    T default_value;
    T value = default_value;
    std::function<bool(const T&)> verifier = [](const T&) { return true; };
    std::vector<std::string> flags = {};
    std::string description = "";
    bool required = false;
    bool is_set = false;

    [[nodiscard]] bool is_required() const { return required; }

    bool set_value(const T& val) {
        if (!verifier(val)) {
            return false;
        }
        value = val;
        is_set = true;
        return true;
    }

    void reset() {
        value = default_value;
        is_set = false;
    }
};

template<typename T>
struct ConfigGroup {
    T config;
    std::string name_;
    
    operator T&() { return config; }
    operator const T&() const { return config; }
    
    [[nodiscard]] const std::string& get_name() const { return name_; }
    
    auto get_fields() { return config.get_fields(); }
    auto get_fields() const { return config.get_fields(); }
};

#define CONFIG_GROUP(Type, var_name) \
    ConfigGroup<Type> var_name{.config = {}, .name_ = #var_name}

}  // namespace lightshield::config
