#pragma once

#include "preset_deserializer.hpp"

#ifdef CONFIGS_LOADER_ENABLE_TOML

#include <toml++/toml.h>
#include <memory>

class TomlDeserializer : public PresetDeserializer {
public:
    void parse_file(const std::string& path) override;
    
    std::optional<std::string> get_string(const std::string& key) const override;
    std::optional<int> get_int(const std::string& key) const override;
    std::optional<bool> get_bool(const std::string& key) const override;
    std::optional<double> get_double(const std::string& key) const override;

private:
    std::unique_ptr<toml::table> m_table;
};

#endif
