#include "CustomMap.h"

#include <fstream>
#include <format>
#include <string>
#include <vector>

#include <RemoteCallAPI.h>
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/mod/NativeMod.h>
#include <ll/api/mod/RegisterHelper.h>
#include <ll/api/service/Bedrock.h>
#include <mc/legacy/ActorUniqueID.h>
#include <mc/server/ServerLevel.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/level/Level.h>
#include <mc/world/level/MapDataManager.h>
#include <mc/world/level/dimension/VanillaDimensions.h>
#include <mc/world/level/saveddata/maps/MapItemSavedData.h>
#include <mc/world/level/storage/LevelStorage.h>
#include <mc/world/level/storage/db_helpers/Category.h>

#define logger custom_map::CustomMap::getInstance().getSelf().getLogger()

namespace custom_map {

namespace {

constexpr int kMapSize       = 128;
constexpr int kMapPixelCount = kMapSize * kMapSize;
constexpr int kFarAwayCoord  = 1000000000;
constexpr auto kRemoteNamespace = "CustomMap";

[[nodiscard]] auto tryGetLevelStorage() -> optional_ref<LevelStorage> {
    return ll::service::getLevel().transform([](Level& level) -> LevelStorage& {
        return level.getLevelStorage();
    });
}

[[nodiscard]] auto tryGetServerLevel() -> optional_ref<ServerLevel> {
    return ll::service::getLevel().transform([](Level& level) -> ServerLevel& {
        return level.asServer();
    });
}

void RemoteCallCleanup() {}

[[nodiscard]] bool MapSetPixels(MapItemSavedData& mapd, std::ifstream& ifs, bool alpha) {
    std::vector<uint> pixels(kMapPixelCount);
    if (!ifs.read(reinterpret_cast<char*>(pixels.data()), sizeof(uint) * pixels.size())) {
        return false;
    }

    if (!alpha) {
        auto alphaBit = 0xffu << 24;
        for (auto& pixel : pixels) {
            pixel |= alphaBit;
        }
    }

    buffer_span<uint> pixelData{};
    pixelData.mBegin = pixels.data();
    pixelData.mEnd   = pixels.data() + pixels.size();

    MapItemSavedData::ChunkBounds bounds{
        .x0 = 0,
        .z0 = 0,
        .x1 = kMapSize,
        .z1 = kMapSize,
    };

    mapd.setMapSection(pixelData, bounds);
    mapd.mLocked = true;
    mapd.setOrigin(
        Vec3(static_cast<float>(kFarAwayCoord), 0.F, static_cast<float>(kFarAwayCoord)),
        0,
        VanillaDimensions::Overworld(),
        false,
        false,
        BlockPos(kFarAwayCoord, 0, kFarAwayCoord)
    );
    return true;
}

[[nodiscard]] long long AddMapFromFile(std::string const& filepath, bool alpha) {
    std::ifstream ifs(filepath, std::ios::binary);
    if (ifs.fail()) {
        return -1LL;
    }

    auto level = tryGetServerLevel();
    if (!level) {
        logger.error("Failed to add map: server level is unavailable");
        return -1LL;
    }

    auto  uid  = level->getNewUniqueID();
    auto& mapd = level->getMapDataManager()->createMapSavedData(uid);
    mapd.mScale = 4;

    if (!MapSetPixels(mapd, ifs, alpha)) {
        logger.error("Failed to add map from '{}': expected a 128x128 RGBA binary file", filepath);
        return -1LL;
    }

    mapd.save(level->getLevelStorage());
    return uid.rawID;
}

} // namespace

struct MapParams {
    std::string                 filename;
    ll::command::Optional<bool> alpha;
    ll::command::Optional<bool> output;
};

void RegisterMapCommands() {
    auto& command = ll::command::CommandRegistrar::getInstance(false)
                        .getOrCreateCommand("map", "Customize the pixels on the map", CommandPermissionLevel::Any);

    command.overload<MapParams>()
        .required("filename")
        .optional("alpha")
        .optional("output")
        .execute([](CommandOrigin const& origin, CommandOutput& output, MapParams const& param) {
            auto* entity = origin.getEntity();
            if (entity == nullptr || !entity->isPlayer()) {
                output.error("Only players can use this command");
                return;
            }

            auto* player = static_cast<Player*>(entity);
            auto* level  = origin.getLevel();
            if (level == nullptr) {
                output.error("Level is not available");
                return;
            }

            auto& item = player->getCarriedItem();
            auto* data = item.mUserData.get();

            if (data == nullptr) {
                output.error("You must hold a filled map in your hand");
                return;
            }

            auto* mapd = level->getMapSavedData(*data);
            if (mapd == nullptr) {
                output.error("You must hold a filled map in your hand");
                return;
            }

            bool const useAlpha   = param.alpha.has_value() ? param.alpha.value() : false;
            bool const showOutput = param.output.has_value() ? param.output.value() : true;

            std::ifstream ifs(param.filename + ".bin", std::ios::binary);
            if (ifs.fail()) {
                ifs.open(param.filename, std::ios::binary);
                if (ifs.fail()) {
                    output.error("No such file or directory.");
                    return;
                }
            }

            if (!MapSetPixels(*mapd, ifs, useAlpha)) {
                output.error("Failed to read map pixel data. Expected a 128x128 RGBA binary file.");
                return;
            }

            mapd->save(level->getLevelStorage());

            if (showOutput) {
                output.success("Map data has been updated");
            } else {
                output.mSuccessCount++;
            }
        });
}

void RemoteCallExport() {
    RemoteCall::exportAs(kRemoteNamespace, "delMap", [](long long uuid) {
        auto storage = tryGetLevelStorage();
        if (!storage) {
            logger.error("Failed to delete map: level storage is unavailable");
            return false;
        }

        std::string mapKey = std::format("map_{}", uuid);
        // 使用 ::DBHelpers::Category::Item
        if (storage->hasKey(mapKey, ::DBHelpers::Category::Item)) {
            storage->deleteData(mapKey, ::DBHelpers::Category::Item);
            return true;
        }
        return false;
    });

    RemoteCall::exportAs(kRemoteNamespace, "getMapList", []() {
        std::vector<long long> uuids;
        auto storage = tryGetLevelStorage();
        if (!storage) {
            logger.error("Failed to enumerate maps: level storage is unavailable");
            return uuids;
        }

        storage->forEachKeyWithPrefix("map_", ::DBHelpers::Category::Item, [&](std::string_view keyLeft, std::string_view) {
            try {
                uuids.push_back(std::stoll(std::string{keyLeft}));
            } catch (std::exception const& e) {
                logger.error(e.what());
            }
        });
        return uuids;
    });

    RemoteCall::exportAs(kRemoteNamespace, "addMap", [](std::string const& filepath) {
        return AddMapFromFile(filepath, true);
    });

    RemoteCall::exportAs(kRemoteNamespace, "addMapNoAlpha", [](std::string const& filepath) {
        return AddMapFromFile(filepath, false);
    });
}

CustomMap& CustomMap::getInstance() {
    static CustomMap instance;
    return instance;
}

bool CustomMap::load() {
    getSelf().getLogger().info("loading...");
    return true;
}

bool CustomMap::enable() {
    getSelf().getLogger().info("enabling...");
    RemoteCallExport();
    RegisterMapCommands();
    return true;
}

bool CustomMap::disable() {
    getSelf().getLogger().info("disabling...");
    RemoteCallCleanup();
    return true;
}

bool CustomMap::unload() {
    getSelf().getLogger().info("unloading...");
    RemoteCallCleanup();
    return true;
}

} // namespace custom_map

LL_REGISTER_MOD(custom_map::CustomMap, custom_map::CustomMap::getInstance());
