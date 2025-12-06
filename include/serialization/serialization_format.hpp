#pragma once

#include <cstdint>

namespace lightshield::config {


enum class SerializationFormat : uint8_t {
    CLI,
    TOML
};

}  // namespace lightshield::config
