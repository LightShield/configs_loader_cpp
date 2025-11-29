#pragma once

#include "help_colors.hpp"
#include <string>
#include <vector>
#include <sstream>

template<typename ConfigsType>
class HelpGenerator {
    const ConfigsType& m_configs;
    bool m_use_colors;
    bool m_enable_interactive;

public:
    HelpGenerator(const ConfigsType& configs, bool use_colors, bool enable_interactive)
        : m_configs(configs), m_use_colors(use_colors), m_enable_interactive(enable_interactive) {}

    std::string generate(const std::string& program_name, size_t max_width, const std::string& filter) const;

private:
    std::string generate_navigation(const std::string& program_name) const;
    std::string generate_groups(const std::string& program_name) const;
    std::string generate_required(const std::string& program_name) const;
    std::string generate_filtered(const std::string& program_name, const std::string& group_filter) const;
    std::string generate_filters(const std::string& program_name) const;
    
    void collect_group_names(std::vector<std::string>& names, const std::string& prefix = "") const;
    void wrap_text(std::ostringstream& out, const std::string& text, size_t indent_col, size_t max_width) const;
    
    template<typename T> void append_usage_field(std::ostringstream& usage, const Config<T>& field) const;
    template<typename T> void append_usage_field(std::ostringstream& usage, const ConfigGroup<T>& group) const;
    template<typename T> void print_field_hierarchical(std::ostringstream& out, const Config<T>& field, size_t indent, size_t max_width, const std::string& prefix = "") const;
    template<typename T> void print_field_hierarchical(std::ostringstream& out, const ConfigGroup<T>& group, size_t indent, size_t max_width, const std::string& prefix = "") const;
    template<typename T> void print_field_if_required(std::ostringstream& out, const Config<T>& field, const std::string& prefix = "") const;
    template<typename T> void print_field_if_required(std::ostringstream& out, const ConfigGroup<T>& group, const std::string& prefix = "") const;
    template<typename T> void print_group_structure(std::ostringstream& out, const Config<T>& field, size_t indent, const std::string& prefix = "") const;
    template<typename T> void print_group_structure(std::ostringstream& out, const ConfigGroup<T>& group, size_t indent, const std::string& prefix = "") const;
    template<typename T> bool print_field_if_matches(std::ostringstream& out, const Config<T>& field, const std::string& filter, const std::string& prefix = "") const;
    template<typename T> bool print_field_if_matches(std::ostringstream& out, const ConfigGroup<T>& group, const std::string& filter, const std::string& prefix = "") const;
    template<typename T> void collect_group_names_from_field(const Config<T>& field, std::vector<std::string>& names, const std::string& prefix) const;
    template<typename T> void collect_group_names_from_field(const ConfigGroup<T>& group, std::vector<std::string>& names, const std::string& prefix) const;
};

#include "help_generator_impl.hpp"
