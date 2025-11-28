#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

template<typename T>
struct ConfigField {
    T default_value;
    std::vector<std::string> flags = {};
    bool required = false;
    std::function<bool(const T&)> verifier = [](const T&) { return true; };

    // Runtime state (initialized after construction)
    T m_value = default_value;
    uint8_t m_is_set = 0u;

    [[nodiscard]] const T& value() const { return m_value; }
    [[nodiscard]] bool is_set() const { return m_is_set != 0u; }
    [[nodiscard]] bool is_required() const { return required; }

    bool set_value(const T& val) {
        if (!verifier(val)) {
            return false;
        }
        m_value = val;
        m_is_set = 1u;
        return true;
    }

    void reset() {
        m_value = default_value;
        m_is_set = 0u;
    }
};
