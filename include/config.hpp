#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include <type_traits>

namespace lightshield::config {

/**
 * @brief Traits for Enum support.
 * Specialized to be empty for non-enum types to avoid memory bloat.
 */
template<typename T, typename Enable = void>
struct EnumTraits {
    // Empty for non-enums
};

template<typename T>
struct EnumTraits<T, std::enable_if_t<std::is_enum_v<T>>> {
    std::function<T(const std::string&)> parser = nullptr;
    std::function<std::string(const T&)> to_string = nullptr;
};

/**
 * @brief Core configuration field.
 * Designed for high-performance direct memory access.
 */
template<typename T>
struct Config {
    T default_value;
    T value = default_value;
    std::function<bool(const T&)> verifier = [](const T&) { return true; };
    std::vector<std::string> flags = {};
    std::string description = "";
    bool required = false;
    bool m_is_set = false;
    
    // Only pays cost (64-128 bytes) when T is an enum
    [[maybe_unused]] EnumTraits<T> enum_traits = {};

    [[nodiscard]] bool is_set() const { return m_is_set; }
    [[nodiscard]] bool is_required() const { return required; }

    bool set_value(const T& val) {
        if (!verifier(val)) {
            return false;
        }
        value = val;
        m_is_set = true;
        return true;
    }

    void reset() {
        value = default_value;
        m_is_set = false;
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
