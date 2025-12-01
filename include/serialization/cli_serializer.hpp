#pragma once

#include "config_serializer.hpp"

template<typename ConfigsType>
class CliSerializer : public ConfigSerializer<ConfigsType> {
public:
    std::string serialize(const ConfigsType& configs, bool only_changes) const override;

private:
    template<typename T> void serialize_field(std::ostringstream& out, const Config<T>& field, bool only_changes) const;
    template<typename T> void serialize_field(std::ostringstream& out, const ConfigGroup<T>& group, bool only_changes) const;
};

template<typename ConfigsType>
std::string CliSerializer<ConfigsType>::serialize(const ConfigsType& configs, bool only_changes) const {
    std::ostringstream out;
    auto fields = configs.get_fields();
    std::apply([&](auto&... field) {
        ((serialize_field(out, field, only_changes)), ...);
    }, fields);
    return out.str();
}

template<typename ConfigsType>
template<typename T>
void CliSerializer<ConfigsType>::serialize_field(std::ostringstream& out, const Config<T>& field, bool only_changes) const {
    if (field.flags.empty()) return;
    
    if (only_changes && field.value == field.default_value) {
        return;
    }
    
    out << field.flags[0] << "=";
    
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
void CliSerializer<ConfigsType>::serialize_field(std::ostringstream& out, const ConfigGroup<T>& group, bool only_changes) const {
    auto fields = group.get_fields();
    std::apply([&](auto&... field) {
        ((serialize_field(out, field, only_changes)), ...);
    }, fields);
}
