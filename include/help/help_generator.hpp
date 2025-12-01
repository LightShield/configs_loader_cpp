#pragma once

#include "help_colors.hpp"
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

struct HelpFormat {
    std::string program_name = "program";
    bool use_colors = true;
    bool enable_interactive = true;
    size_t max_width = 80;
    bool show_current_values = true;
};

template<typename ConfigsType>
class HelpGenerator {
    const ConfigsType& m_configs;
    const std::string& m_program_name;
    const bool m_use_colors;
    const bool m_enable_interactive;
    const size_t m_max_width;
    const bool m_show_current_values;

public:
    HelpGenerator(const ConfigsType& configs, const HelpFormat& format)
        : m_configs(configs), m_program_name(format.program_name), m_use_colors(format.use_colors), 
          m_enable_interactive(format.enable_interactive), m_max_width(format.max_width), 
          m_show_current_values(format.show_current_values) {}

    std::string generate(const std::string& filter) const;

private:
    std::string generate_navigation() const;
    std::string generate_groups() const;
    std::string generate_required() const;
    std::string generate_filtered(const std::string& group_filter) const;
    std::string generate_filters() const;
    
    void collect_group_names(std::vector<std::string>& names, const std::string& prefix = "") const;
    void wrap_text(std::ostringstream& out, const std::string& text, size_t indent_col, size_t max_width) const;
    
    template<typename T> void append_usage_field(std::ostringstream& usage, const Config<T>& field) const;
    template<typename T> void append_usage_field(std::ostringstream& usage, const ConfigGroup<T>& group) const;
    template<typename T> void print_field_hierarchical(std::ostringstream& out, const Config<T>& field, size_t indent, const std::string& prefix = "") const;
    template<typename T> void print_field_hierarchical(std::ostringstream& out, const ConfigGroup<T>& group, size_t indent, const std::string& prefix = "") const;
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
