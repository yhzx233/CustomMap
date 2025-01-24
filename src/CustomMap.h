#pragma once


#include <ll/api/mod/NativeMod.h>
#include <memory>

namespace CustomMap {

// extern std::shared_ptr<KomomoModManager> komomoModManager;
class Entry {
public:
    static Entry& getInstance();


    Entry() : mSelf(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    bool load();

    bool enable();

    bool disable();

    // bool unload();

private:
    ll::mod::NativeMod& mSelf;
};
}; // namespace CustomMap