#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

template<typename T>
struct Config {
    T default_value;
    std::vector<std::string> flags = {};  // TODO(C++26): Use constexpr container for compile-time validation
    bool required = false;
    std::string description = "";
    std::function<bool(const T&)> verifier = [](const T&) { return true; };

    // Runtime state
    T value = default_value;
    uint8_t m_is_set = 0u;

    [[nodiscard]] bool is_set() const { return m_is_set != 0u; }
    [[nodiscard]] bool is_required() const { return required; }

    bool set_value(const T& val) {
        if (!verifier(val)) {
            return false;
        }
        value = val;
        m_is_set = 1u;
        return true;
    }

    void reset() {
        value = default_value;
        m_is_set = 0u;
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

#define CONFIG_GROUP(Type, name) \
    ConfigGroup<Type> name{.name_ = #name}
