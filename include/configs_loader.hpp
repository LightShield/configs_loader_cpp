#pragma once

#include "config.hpp"
#include "help/help_generator.hpp"
#include "serialization/serialization_format.hpp"
#include <functional>
#include <optional>
#include <string>
#include <tuple>

namespace lightshield::config {

enum class UnknownFlagBehavior : uint8_t {
    Ignore = 0,
    Warn,
    Error
};

// Current macro - requires listing all fields (C++20)
#define REGISTER_CONFIG_FIELDS(...) \
    auto get_fields() { \
        return std::tie(__VA_ARGS__); \
    } \
    auto get_fields() const { \
        return std::tie(__VA_ARGS__); \
    }

// Future macro - will use C++26 reflection to auto-detect fields
// Reserved for future use - DO NOT IMPLEMENT until C++26 is available
#define REGISTER_CONFIG_STRUCT(StructName) \
    static_assert(false, "REGISTER_CONFIG_STRUCT requires C++26 reflection - use REGISTER_CONFIG_FIELDS for now");

/**
 * @brief Orchestrates configuration loading from various sources (CLI, presets, etc.).
 * 
 * @tparam ConfigsType A struct containing lightshield::config::Config fields and a 
 *                     REGISTER_CONFIG_FIELDS(...) macro call.
 */
template<typename ConfigsType>
class ConfigsLoader {
public:
    /// The actual configuration instance
    ConfigsType configs;

    /// Customization options for help generation
    HelpFormat help_format;

    UnknownFlagBehavior unknown_flag_behavior = UnknownFlagBehavior::Error;

    /**
     * @brief Construct a new ConfigsLoader and automatically initialize.
     * 
     * @note This will call std::exit(1) if initialization fails.
     *       Use the default constructor + init() for manual error handling.
     */
    ConfigsLoader(int argc, char* argv[]);

    /**
     * @brief Default constructor for manual initialization.
     */
    ConfigsLoader() = default;

    /**
     * @brief Initialize configurations from command line and presets.
     * 
     * @return int 0 on success, non-zero on failure.
     */
    int init(int argc, char* argv[]);

    /**
     * @brief Check if the loader was successfully initialized.
     */
    [[nodiscard]] bool is_initialized() const;

    /**
     * @brief Get the internal error report if initialization failed.
     */
    [[nodiscard]] std::string get_error() const { return m_error_report; }

    /**
     * @brief Generate a formatted help string for the current configurations.
     */
    [[nodiscard]] std::string generate_help(const std::string& filter = "", 
                                            std::optional<std::reference_wrapper<const HelpFormat>> format = std::nullopt) const;

    /**
     * @brief Dump the current configuration state to a string.
     */
    [[nodiscard]] std::string dump_configs(SerializationFormat format = SerializationFormat::CLI, bool only_changes = false) const;

private:
    bool m_initialized = false;
    std::string m_error_report;
};

}  // namespace lightshield::config

// Include implementation
#include "configs_loader_impl.hpp"
