#pragma once

#include <string>
#include <sstream>

template<typename ConfigsType>
class ConfigSerializer {
protected:
    const ConfigsType& m_configs;
    bool m_only_changes;

public:
    ConfigSerializer(const ConfigsType& configs, bool only_changes)
        : m_configs(configs), m_only_changes(only_changes) {}

    virtual ~ConfigSerializer() = default;
    virtual std::string serialize() const = 0;

protected:
    template<typename T> void serialize_field(std::ostringstream& out, const Config<T>& field) const;
    template<typename T> void serialize_field(std::ostringstream& out, const ConfigGroup<T>& group) const;
};
