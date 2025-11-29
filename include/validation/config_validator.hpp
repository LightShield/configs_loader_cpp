#pragma once

#include <string>
#include <vector>

struct ValidationError {
    std::string field_name;
    std::string flag;
    std::string error_message;
};

template<typename ConfigsType>
class ConfigValidator {
    const ConfigsType& m_configs;
    std::vector<ValidationError> m_errors;

public:
    explicit ConfigValidator(const ConfigsType& configs) : m_configs(configs) {}

    void validate_reserved_flags();
    void validate_required_fields();
    bool has_errors() const { return !m_errors.empty(); }
    std::string get_error_report() const;

private:
    template<typename T> void validate_field(const Config<T>& field, const std::string& prefix = "");
    template<typename T> void validate_field(const ConfigGroup<T>& group, const std::string& prefix = "");
    template<typename T> void check_reserved_flags(const Config<T>& field);
    template<typename T> void check_reserved_flags(const ConfigGroup<T>& group);
};

#include "config_validator_impl.hpp"
