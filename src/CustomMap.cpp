#include "CustomMap.h"

#include <RemoteCallAPI.h>
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/mod/NativeMod.h>
#include <ll/api/mod/RegisterHelper.h>
#include <ll/api/service/Bedrock.h>
#include <mc/common/ActorUniqueID.h>
#include <mc/nbt/Int64Tag.h>
#include <mc/server/ServerLevel.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/item/MapItem.h>
#include <mc/world/level/Level.h>
#include <mc/world/level/MapDataManager.h>
#include <mc/world/level/dimension/VanillaDimensions.h>
#include <mc/world/level/saveddata/maps/MapItemSavedData.h>
#include <mc/world/level/storage/LevelStorage.h>
#include <mc/world/level/storage/db_helpers/Category.h>

#define logger CustomMap::getInstance().getSelf().getLogger()

namespace custom_map {

struct MapParams {
    std::string filename;
    bool        alpha{false};
    bool        output{true};
};

void MapSetPixels(MapItemSavedData& mapd, std::ifstream& ifs, bool alpha) {

    mapd.setPixel(0, 0, 0);
    mapd.setPixel(0, 127, 127);

    auto pixels = const_cast<unsigned int*>(mapd.getPixels().mBegin);
    ifs.read(reinterpret_cast<char*>(pixels), sizeof(unsigned int) * 128 * 128);

    if (!alpha) {
        auto alpha_bit = 0xff << 24;
        for (int i = 0; i < 128 * 128; i++) {
            pixels[i] |= alpha_bit;
        }
    }

    mapd.setLocked();
    mapd.setOrigin(
        Vec3(1e9, 0., 1e9),
        0,
        VanillaDimensions::Overworld(),
        false,
        false,
        BlockPos((int)1e9, 0, (int)1e9)
    );
}


void RegisterMapCommands() {
    auto& command = ll::command::CommandRegistrar::getInstance()
                        .getOrCreateCommand("map", "Customize the pixels on the map", CommandPermissionLevel::Any);
    command.overload<MapParams>()
        .required("filename")
        .optional("alpha")
        .optional("output")
        .execute([&](CommandOrigin const& origin, CommandOutput& output, MapParams const& param, Command const&) {
            auto* entity = origin.getEntity();
            if (entity == nullptr || !entity->isType(ActorType::Player)) {
                output.error("Only players can use this command");
                return;
            }

            auto* player = static_cast<Player*>(entity);
            auto* level  = origin.getLevel();

            auto& item = player->getCarriedItem();
            auto* data = item.getUserData();

            if (data == nullptr) {
                output.error("You must hold a filled map in your hand");
                return;
            }

            auto* mapd = level->getMapSavedData(*data);

            if (mapd == nullptr) {
                output.error("You must hold a filled map in your hand");
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
        });
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
        db.forEachKeyWithPrefix("map_", DBHelpers::Category::Item, [&](std::string_view key_left, std::string_view) {
            try {
                uuids.push_back(std::stoll(key_left.data()));
            } catch (std::exception& e) {
                logger.error(e.what());
                return;
            }
        });
        return uuids;
    });

    auto addMap = [](const std::string& filepath, bool alpha) {
        std::ifstream ifs(filepath, std::ios::binary);
        if (ifs.fail()) {
            return -1LL;
        }

        auto& level = static_cast<ServerLevel&>(ll::service::getLevel().get());
        auto  uid   = level.getNewUniqueID();
        auto& mapd  = level._getMapDataManager().createMapSavedData(uid);

        mapd.setScale(4); // no parentMapId
        MapSetPixels(mapd, ifs, alpha);

        mapd.save(level.getLevelStorage());
        return uid.rawID;
    };

    RemoteCall::exportAs("CustomMap", "addMap", [&addMap](const std::string& filepath) {
        return addMap(filepath, true);
    });

    RemoteCall::exportAs("CustomMap", "addMapNoAlpha", [&addMap](const std::string& filepath) {
        return addMap(filepath, false);
    });
}

CustomMap& CustomMap::getInstance() {
    static CustomMap instance;
    return instance;
}

bool CustomMap::load() {
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

LL_REGISTER_MOD(custom_map::CustomMap, custom_map::CustomMap::getInstance());

} // namespace custom_map
