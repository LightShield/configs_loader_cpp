#pragma once

#include "cli_serializer.hpp"
#include "config_serializer.hpp"
#include "serialization_format.hpp"
#include "toml_serializer.hpp"
#include <memory>

template<typename ConfigsType>
class SerializerFactory {
public:
    static std::unique_ptr<ConfigSerializer<ConfigsType>> create(const ConfigsType& configs, const SerializationFormat format, const bool only_changes) {
        switch (format) {
            case SerializationFormat::CLI:
                return std::make_unique<CliSerializer<ConfigsType>>(configs, only_changes);
            case SerializationFormat::TOML:
                return std::make_unique<TomlSerializer<ConfigsType>>(configs, only_changes);
        }
        return nullptr;
    }
};
