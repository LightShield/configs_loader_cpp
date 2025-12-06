#pragma once

#include <string>
#include <sstream>

namespace lightshield::config {


template<typename ConfigsType>
class ConfigSerializer {
public:
    virtual ~ConfigSerializer() = default;
    virtual std::string serialize(const ConfigsType& configs, bool only_changes) const = 0;

protected:
    template<typename T> void serialize_field(std::ostringstream& out, const Config<T>& field, bool only_changes) const;
    template<typename T> void serialize_field(std::ostringstream& out, const ConfigGroup<T>& group, bool only_changes) const;
};

}  // namespace lightshield::config
