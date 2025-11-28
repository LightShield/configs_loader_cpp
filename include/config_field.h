#pragma once

#include <string>
#include <vector>
#include <functional>
#include <optional>

template<typename T>
class ConfigField {
public:
    struct Metadata {
        T default_value;
        std::vector<std::string> flags;
        bool required = false;
        std::function<bool(const T&)> verifier = [](const T&) { return true; };
    };

    explicit ConfigField(Metadata meta) 
        : m_metadata(std::move(meta))
        , m_value(m_metadata.default_value) {}

    [[nodiscard]] const T& value() const { return m_value; }
    [[nodiscard]] bool is_set() const { return m_is_set; }
    [[nodiscard]] bool is_required() const { return m_metadata.required; }
    [[nodiscard]] const std::vector<std::string>& flags() const { return m_metadata.flags; }

    bool set_value(const T& val) {
        if (!m_metadata.verifier(val)) {
            return false;
        }
        m_value = val;
        m_is_set = true;
        return true;
    }

    void reset() {
        m_value = m_metadata.default_value;
        m_is_set = false;
    }

private:
    Metadata m_metadata;
    T m_value;
    bool m_is_set = false;
};
