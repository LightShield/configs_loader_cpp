#pragma once

#include "config_serializer.hpp"

template<typename ConfigsType>
class TomlSerializer : public ConfigSerializer<ConfigsType> {
public:
    using ConfigSerializer<ConfigsType>::ConfigSerializer;

    std::string serialize() const override {
        std::ostringstream out;
        auto fields = this->m_configs.get_fields();
        std::apply([&](auto&... field) {
            ((this->serialize_field(out, field)), ...);
        }, fields);
        return out.str();
    }

protected:
    template<typename T> void serialize_field(std::ostringstream& out, const Config<T>& field) const;
    template<typename T> void serialize_field(std::ostringstream& out, const ConfigGroup<T>& group) const;
};

template<typename ConfigsType>
template<typename T>
void TomlSerializer<ConfigsType>::serialize_field(std::ostringstream& out, const Config<T>& field) const {
    if (field.flags.empty()) return;
    
    if (this->m_only_changes && field.value == field.default_value) {
        return;
    }
    
    std::string key = field.flags[0];
    if (key.starts_with("--")) {
        key = key.substr(2);
    } else if (key.starts_with("-")) {
        key = key.substr(1);
    }
    
    out << key << " = ";
    
    if constexpr (std::is_same_v<T, std::string>) {
        out << "\"" << field.value << "\"";
    } else if constexpr (std::is_same_v<T, bool>) {
        out << (field.value ? "true" : "false");
    } else {
        out << field.value;
    }
    
    out << "\n";
}

template<typename ConfigsType>
template<typename T>
void TomlSerializer<ConfigsType>::serialize_field(std::ostringstream& out, const ConfigGroup<T>& group) const {
    auto fields = group.config.get_fields();
    std::apply([&](auto&... field) {
        ((serialize_field(out, field)), ...);
    }, fields);
}
