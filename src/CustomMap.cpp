#include "CustomMap.h"

#include "mc/world/level/saveddata/maps/MapItemSavedData.h"

#include <RemoteCallAPI.h>
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/plugin/NativePlugin.h>
#include <ll/api/service/Bedrock.h>
#include <mc/enums/d_b_helpers/Category.h>
#include <mc/server/ServerLevel.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/world/ActorUniqueID.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/components/MapDataManager.h>
#include <mc/world/item/MapItem.h>
#include <mc/world/level/Level.h>
#include <mc/world/level/dimension/VanillaDimensions.h>
#include <mc/world/level/storage/LevelStorage.h>

#define logger CustomMap::getInstance().getSelf().getLogger()

namespace custom_map {

struct MapParams {
    std::string filename;
    bool        alpha{false};
    bool        output{true};
};

void MapSetPixels(MapItemSavedData& mapd, std::ifstream& ifs, bool alpha) {
    auto pixels = const_cast<unsigned int*>(mapd.getPixels().mBegin);
    ifs.read(reinterpret_cast<char*>(pixels), sizeof(unsigned int) * 128 * 128);

    if (!alpha) {
        auto alpha_bit = 0xff << 24;
        for (int i = 0; i < 128 * 128; i++) {
            pixels[i] |= alpha_bit;
        }
    }

    mapd.setPixelDirty(0, 0);
    mapd.setPixelDirty(127, 127);

    mapd.setLocked();
    mapd.setOrigin(Vec3(1e9, 0., 1e9), 0, VanillaDimensions::Overworld, false, false, BlockPos((int)1e9, 0, (int)1e9));
}


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

            MapSetPixels(*mapd, ifs, param.alpha);

            mapd->save(level->getLevelStorage());

            if (param.output) {
                output.success("Map data has been updated");
            } else {
                output.success();
            }
        }>();
}

void RemoteCallExport() {
    RemoteCall::exportAs("CustomMap", "delMap", [](long long uuid) {
        std::string mapKey = std::format("map_{}", uuid);
        auto&       db     = ll::service::getLevel()->getLevelStorage();
        if (db.hasKey(mapKey, DBHelpers::Category::Item)) {
            db.deleteData(mapKey, DBHelpers::Category::Item);
            return true;
        } else {
            return false;
        }
    });

    RemoteCall::exportAs("CustomMap", "getMapList", []() {
        std::vector<long long> uuids;
        auto&                  db = ll::service::getLevel()->getLevelStorage();
        db.forEachKeyWithPrefix(
            "map_",
            DBHelpers::Category::Item,
            [&](std::string_view key_left, std::string_view data) {
                try {
                    uuids.push_back(std::stoll(key_left.data()));
                } catch (std::exception& e) {
                    logger.error(e.what());
                    return;
                }
            }
        );
        return uuids;
    });

    RemoteCall::exportAs("CustomMap", "addMap", [](const std::string& filepath) {
        std::ifstream ifs(filepath, std::ios::binary);
        if (ifs.fail()) {
            return -1LL;
        }

        auto& level = static_cast<ServerLevel&>(ll::service::getLevel().get());
        auto  id    = level.getNewUniqueID();
        auto& mapd  = level._getMapDataManager().createMapSavedData(id);

        mapd.setScale(4); // no parentMapId
        MapSetPixels(mapd, ifs, true);

        mapd.save(level.getLevelStorage());
        return id.get();
    });
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
    RemoteCallExport();

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
