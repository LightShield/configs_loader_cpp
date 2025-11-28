#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

template<typename T>
class ConfigField {
public:
    ConfigField(T default_val, 
                std::vector<std::string> flag_list = {}, 
                bool is_required = false,
                std::function<bool(const T&)> verify_fn = [](const T&) { return true; })
        : m_verifier(std::move(verify_fn))
        , m_flags(std::move(flag_list))
        , m_value(std::move(default_val))
        , m_default_value(m_value)
        , m_required(is_required ? 1u : 0u)
        , m_is_set(0u) {}

    [[nodiscard]] const T& value() const { return m_value; }
    [[nodiscard]] bool is_set() const { return m_is_set != 0u; }
    [[nodiscard]] bool is_required() const { return m_required != 0u; }
    [[nodiscard]] const std::vector<std::string>& flags() const { return m_flags; }

    bool set_value(const T& val) {
        if (!m_verifier(val)) {
            return false;
        }
        m_value = val;
        m_is_set = 1u;
        return true;
    }

    void reset() {
        m_value = m_default_value;
        m_is_set = 0u;
    }

private:
    std::function<bool(const T&)> m_verifier;
    std::vector<std::string> m_flags;
    T m_value;
    T m_default_value;
    uint8_t m_required;
    uint8_t m_is_set;
};
