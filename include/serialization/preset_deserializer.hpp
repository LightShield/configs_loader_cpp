#pragma once

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

class PresetDeserializer {
public:
    virtual ~PresetDeserializer() = default;
    
    virtual void parse_file(const std::string& path) = 0;
    
    virtual std::optional<std::string> get_string(const std::string& key) const = 0;
    virtual std::optional<int> get_int(const std::string& key) const = 0;
    virtual std::optional<bool> get_bool(const std::string& key) const = 0;
    virtual std::optional<double> get_double(const std::string& key) const = 0;
};

std::unique_ptr<PresetDeserializer> create_preset_deserializer(const std::string& file_path);
