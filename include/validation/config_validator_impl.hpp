#pragma once

#include "config.hpp"
#include <sstream>

template<typename ConfigsType>
void ConfigValidator<ConfigsType>::validate_reserved_flags() {
    m_errors.clear();
    
    auto fields = m_configs.get_fields();
    std::apply([&](auto&... field) {
        ((check_reserved_flags(field)), ...);
    }, fields);
}

template<typename ConfigsType>
void ConfigValidator<ConfigsType>::validate_required_fields() {
    auto fields = m_configs.get_fields();
    std::apply([&](auto&... field) {
        ((validate_field(field)), ...);
    }, fields);
}

template<typename ConfigsType>
std::string ConfigValidator<ConfigsType>::get_error_report() const {
    if (m_errors.empty()) return "";
    
    std::ostringstream report;
    report << "Configuration validation failed with " << m_errors.size() << " error(s):\n\n";
    
    for (const auto& error : m_errors) {
        report << "  • " << error.error_message;
        if (!error.flag.empty()) {
            report << " (flag: " << error.flag << ")";
        }
        report << "\n";
    }
    
    return report.str();
}

template<typename ConfigsType>
template<typename T>
void ConfigValidator<ConfigsType>::validate_field(const Config<T>& field, const std::string& prefix) {
    if (field.is_required() && !field.is_set()) {
        const std::string flag = field.flags.empty() ? "unknown" : 
            (prefix.empty() ? field.flags[0] : "--" + prefix + "." + field.flags[0].substr(2));
        
        m_errors.push_back({
            .field_name = prefix.empty() ? "field" : prefix,
            .flag = flag,
            .error_message = "Required field '" + flag + "' is not set"
        });
    }
}

template<typename ConfigsType>
template<typename T>
void ConfigValidator<ConfigsType>::validate_field(const ConfigGroup<T>& group, const std::string& prefix) {
    const std::string full_prefix = prefix.empty() ? group.get_name() : prefix + "." + group.get_name();
    auto fields = group.get_fields();
    std::apply([&](auto&... field) {
        ((validate_field(field, full_prefix)), ...);
    }, fields);
}

template<typename ConfigsType>
template<typename T>
void ConfigValidator<ConfigsType>::check_reserved_flags(const Config<T>& field) {
    for (const auto& flag : field.flags) {
        if (flag == "--preset" || flag == "-p") {
            m_errors.push_back({
                .field_name = "field",
                .flag = flag,
                .error_message = "Config field cannot use reserved flag '" + flag + "'"
            });
        }
    }
}

template<typename ConfigsType>
template<typename T>
void ConfigValidator<ConfigsType>::check_reserved_flags(const ConfigGroup<T>& group) {
    auto fields = group.get_fields();
    std::apply([&](auto&... field) {
        ((check_reserved_flags(field)), ...);
    }, fields);
}
