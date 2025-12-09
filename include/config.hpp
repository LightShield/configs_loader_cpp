#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace lightshield::config {

template<typename T>
struct EnumTraits {
    std::function<T(const std::string&)> parser = nullptr;
    std::function<std::string(const T&)> to_string = nullptr;
};

template<typename T>
struct Config {
    T default_value;
    T value = default_value;
    std::function<bool(const T&)> verifier = [](const T&) { return true; };
    std::vector<std::string> flags = {};
    std::string description = "";
    bool required = false;
    uint8_t m_is_set = 0u;
    EnumTraits<T> enum_traits = {};  // Only used when T is enum

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

}  // namespace lightshield::config
