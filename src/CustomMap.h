#pragma once

#include <ll/api/plugin/NativePlugin.h>

namespace custom_map {

class CustomMap {
    CustomMap();

public:
    CustomMap(CustomMap&&)                 = delete;
    CustomMap(const CustomMap&)            = delete;
    CustomMap& operator=(CustomMap&&)      = delete;
    CustomMap& operator=(const CustomMap&) = delete;

    static CustomMap& getInstance();

    [[nodiscard]] ll::plugin::NativePlugin& getSelf() const;

    /// @return True if the plugin is loaded successfully.
    bool load(ll::plugin::NativePlugin&);

    /// @return True if the plugin is enabled successfully.
    bool enable();

    /// @return True if the plugin is disabled successfully.
    bool disable();

private:
    ll::plugin::NativePlugin* mSelf{};
};

} // namespace custom_map
