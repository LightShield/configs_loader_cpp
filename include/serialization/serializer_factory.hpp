#pragma once

#include "cli_serializer.hpp"
#include "serialization_format.hpp"
#include "toml_serializer.hpp"
#include <string>

template<typename ConfigsType>
class SerializerFactory {
public:
    static std::string serialize(const ConfigsType& configs, const SerializationFormat format, const bool only_changes) {
        switch (format) {
            case SerializationFormat::CLI: {
                CliSerializer<ConfigsType> serializer(configs, only_changes);
                return serializer.serialize();
            }
            case SerializationFormat::TOML: {
                TomlSerializer<ConfigsType> serializer(configs, only_changes);
                return serializer.serialize();
            }
        }
        return "";
    }
};
