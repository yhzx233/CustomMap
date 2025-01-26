#pragma once

#include <ll/api/mod/NativeMod.h>

namespace custom_map {

class CustomMap {

public:
    CustomMap(CustomMap&&)                 = delete;
    CustomMap(const CustomMap&)            = delete;
    CustomMap& operator=(CustomMap&&)      = delete;
    CustomMap& operator=(const CustomMap&) = delete;

    static CustomMap& getInstance();

    CustomMap() : mSelf(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    /// @return True if the plugin is loaded successfully.
    bool load();

    /// @return True if the plugin is enabled successfully.
    bool enable();

    /// @return True if the plugin is disabled successfully.
    bool disable();

private:
    ll::mod::NativeMod& mSelf;
};

} // namespace custom_map
