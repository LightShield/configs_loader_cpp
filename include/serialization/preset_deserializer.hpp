#pragma once

#include <string>
#include <optional>
#include <stdexcept>

// Abstract interface for preset file parsers
class PresetParser {
public:
    virtual ~PresetParser() = default;
    
    // Load preset file and return key-value pairs
    virtual void parse_file(const std::string& path) = 0;
    
    // Get value for a key
    virtual std::optional<std::string> get_string(const std::string& key) const = 0;
    virtual std::optional<int> get_int(const std::string& key) const = 0;
    virtual std::optional<bool> get_bool(const std::string& key) const = 0;
    virtual std::optional<double> get_double(const std::string& key) const = 0;
};

// Factory function to create parser based on file extension
std::unique_ptr<PresetParser> create_preset_parser(const std::string& file_path);
