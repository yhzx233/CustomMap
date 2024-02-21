#include "CustomMap.h"

#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/plugin/NativePlugin.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/world/ActorUniqueID.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/item/MapItem.h>
#include <mc/world/level/Level.h>
#include <mc/world/level/dimension/VanillaDimensions.h>
#include <memory>

namespace custom_map {

struct MapParams {
    std::string filename;
    bool        alpha{false};
    bool        output{true};
};

void RegisterMapCommands() {
    auto& command = ll::command::CommandRegistrar::getInstance()
                        .getOrCreateCommand("map", "Customize the pixels on the map", CommandPermissionLevel::Any);
    command.overload<MapParams>()
        .required("filename")
        .optional("alpha")
        .optional("output")
        .execute<[&](CommandOrigin const& origin, CommandOutput& output, MapParams const& param, Command const&) {
            auto* entity = origin.getEntity();
            if (entity == nullptr || !entity->isType(ActorType::Player)) {
                output.error("Only players can use this command");
                return;
            }

            auto* player = static_cast<Player*>(entity);

            auto& item  = player->getCarriedItem();
            auto* level = origin.getLevel();
            auto  uuid  = MapItem::getMapId(item.getUserData());
            auto* mapd  = level->getMapSavedData(uuid);

            if (mapd == nullptr) {
                output.error("This map has no data");
                return;
            }

            std::ifstream ifs(param.filename + ".bin", std::ios::binary);
            if (ifs.fail()) {
                ifs.open(param.filename, std::ios::binary);
                if (ifs.fail()) {
                    output.error("No such file or directory.");
                    return;
                }
            }

            auto pixels = const_cast<unsigned int*>(mapd->getPixels().mBegin);
            ifs.read(reinterpret_cast<char*>(pixels), sizeof(unsigned int) * 128 * 128);

            if (!param.alpha) {
                auto alpha_bit = 0xff << 24;
                for (int i = 0; i < 128 * 128; i++) {
                    pixels[i] |= alpha_bit;
                }
            }

            mapd->setPixelDirty(0, 0);
            mapd->setPixelDirty(127, 127);

            mapd->setLocked();
            mapd->setOrigin(
                Vec3(1e9, 0., 1e9),
                0,
                VanillaDimensions::Overworld,
                false,
                false,
                BlockPos((int)1e9, 0, (int)1e9)
            );
            mapd->save(level->getLevelStorage());

            if (param.output) {
                output.success("Map data has been updated");
            } else {
                output.success();
            }
        }>();
}

CustomMap::CustomMap() = default;

CustomMap& CustomMap::getInstance() {
    static CustomMap instance;
    return instance;
}

ll::plugin::NativePlugin& CustomMap::getSelf() const { return *mSelf; }

bool CustomMap::load(ll::plugin::NativePlugin& self) {
    mSelf = std::addressof(self);
    getSelf().getLogger().info("loading...");

    // Code for loading the plugin goes here.

    return true;
}

bool CustomMap::enable() {
    getSelf().getLogger().info("enabling...");

    // Code for enabling the plugin goes here.
    RegisterMapCommands();

    return true;
}

bool CustomMap::disable() {
    getSelf().getLogger().info("disabling...");

    // Code for disabling the plugin goes here.

    return true;
}

extern "C" {
_declspec(dllexport) bool ll_plugin_load(ll::plugin::NativePlugin& self) { return CustomMap::getInstance().load(self); }

_declspec(dllexport) bool ll_plugin_enable(ll::plugin::NativePlugin&) { return CustomMap::getInstance().enable(); }

_declspec(dllexport) bool ll_plugin_disable(ll::plugin::NativePlugin&) { return CustomMap::getInstance().disable(); }

/// @warning Unloading the plugin may cause a crash if the plugin has not released all of its
/// resources. If you are unsure, keep this function commented out.
// _declspec(dllexport) bool ll_plugin_unload(ll::plugin::NativePlugin&) {
//     return CustomMap::getInstance().unload();
// }
}

} // namespace custom_map
