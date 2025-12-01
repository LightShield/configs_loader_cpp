#pragma once

#include "cli_serializer.hpp"
#include "config_serializer.hpp"
#include "serialization_format.hpp"
#include "toml_serializer.hpp"
#include <memory>

template<typename ConfigsType>
class SerializerFactory {
public:
    static std::unique_ptr<ConfigSerializer<ConfigsType>> create(const SerializationFormat format) {
        switch (format) {
            case SerializationFormat::CLI:
                return std::make_unique<CliSerializer<ConfigsType>>();
            case SerializationFormat::TOML:
                return std::make_unique<TomlSerializer<ConfigsType>>();
        }
        return nullptr;
    }
};
