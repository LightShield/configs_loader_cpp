#include "serialization/toml_deserializer.hpp"

#ifdef CONFIGS_LOADER_ENABLE_TOML

namespace lightshield::config {

void TomlDeserializer::parse_file(const std::string& path) {
    try {
        m_table = std::make_unique<toml::table>(toml::parse_file(path));
    } catch (const toml::parse_error& err) {
        throw std::runtime_error(std::string("TOML parse error: ") + err.what());
    }
}

std::optional<std::string> TomlDeserializer::get_string(const std::string& key) const {
    if (!m_table) return std::nullopt;
    auto node = (*m_table)[key];
    if (auto val = node.value<std::string>()) {
        return *val;
    }
    return std::nullopt;
}

std::optional<int> TomlDeserializer::get_int(const std::string& key) const {
    if (!m_table) return std::nullopt;
    auto node = (*m_table)[key];
    if (auto val = node.value<int64_t>()) {
        return static_cast<int>(*val);
    }
    return std::nullopt;
}

std::optional<bool> TomlDeserializer::get_bool(const std::string& key) const {
    if (!m_table) return std::nullopt;
    auto node = (*m_table)[key];
    if (auto val = node.value<bool>()) {
        return *val;
    }
    return std::nullopt;
}

std::optional<double> TomlDeserializer::get_double(const std::string& key) const {
    if (!m_table) return std::nullopt;
    auto node = (*m_table)[key];
    if (auto val = node.value<double>()) {
        return *val;
    }
    return std::nullopt;
}

}  // namespace lightshield::config

#endif
