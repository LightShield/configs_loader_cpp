#pragma once

#include <sstream>

#include "config.hpp"

namespace lightshield::config {

template<typename ConfigsType>
void ConfigValidator<ConfigsType>::validate_reserved_flags() {
    m_errors.clear();
    
    // TODO(C++26): Move to compile-time validation using reflection
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
    std::string flag;
    if (field.flags.empty()) {
        flag = "unknown";
    } else if (prefix.empty()) {
        flag = field.flags[0];
    } else {
        flag = "--" + prefix + "." + field.flags[0].substr(2);
    }
    
    if (field.is_required() && !field.is_set) {
        std::string msg = "Required field '" + flag + "' is not set";
        if (!field.description.empty()) {
            msg += " [" + field.description + "]";
        }
        
        m_errors.push_back({
            .field_name = prefix.empty() ? "field" : prefix,
            .flag = flag,
            .error_message = msg
        });
    }
    
    if (field.is_set && !field.verifier(field.value)) {
        std::string msg = "Validation failed for field '" + flag + "'";
        if (!field.description.empty()) {
            msg += " [" + field.description + "]";
        }
        
        if constexpr (std::is_same_v<T, std::string>) {
            msg += ": value = \"" + field.value + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            msg += ": value = " + std::string(field.value ? "true" : "false");
        } else {
            msg += ": value = " + std::to_string(field.value);
        }
        
        m_errors.push_back({
            .field_name = prefix.empty() ? "field" : prefix,
            .flag = flag,
            .error_message = msg
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
    // TODO(C++26): This should be a static_assert with reflection
    for (const auto& flag : field.flags) {
        if (flag == "--preset") {
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

}  // namespace lightshield::config
