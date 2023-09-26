#include "pch.h"
#include <llapi/EventAPI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/RemoteCallAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/LLAPI.h>

#include <llapi/mc/Level.hpp>
#include <llapi/mc/BlockInstance.hpp>
#include <llapi/mc/Block.hpp>
#include <llapi/mc/BlockSource.hpp>
#include <llapi/mc/Actor.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/ItemStack.hpp>

#include <llapi/mc/ServerPlayer.hpp>
#include <llapi/mc/ActorUniqueID.hpp>
#include <llapi/mc/MapItemSavedData.hpp>
#include <llapi/mc/MapItem.hpp>
#include <llapi/mc/DBStorage.hpp>
#include <llapi/mc/Int64Tag.hpp>

#define ENABLE_VIRTUAL_FAKESYMBOL_SERVERLEVEL
#include <llapi/mc/ServerLevel.hpp>
#include <llapi/mc/MapDataManager.hpp>


#include <llapi/Utils/Bstream.h>

#include <iostream>
#include <fstream>

Logger logger("CustomMap");

static inline string ifs2str(std::ifstream& ifs) {
	return { (std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>() };
}

class MapCommand : public Command {
	string mFilename;
	bool mFilename_isSet;
	bool mAlpha;
	bool mAlpha_isSet;
	bool mOutput;
	bool mOutput_isSet;

public:
	void execute(CommandOrigin const& ori, CommandOutput& output) const override {//执行部分
		ServerPlayer* player = ori.getPlayer();
		if (!player->isPlayer()) {
			output.error("Not a player");
			return;
		}
		auto& item = player->getCarriedItem();
		auto level = ori.getLevel();
		ActorUniqueID id = MapItem::getMapId(item.getUserData());
		/**
		ActorUniqueID id;
		SymCall("?getMapId@MapItem@@SA?AUActorUniqueID@@PEBVCompoundTag@@@Z", ActorUniqueID*, ActorUniqueID*, uintptr_t, uintptr_t)(
			&id,
			*(((uintptr_t*)&item) + 2),
			(uintptr_t)level);
		**/
		MapItemSavedData* mapd = level->getMapSavedData(id);
		if (!mapd) { output.error("Not holding a filled map."); return; }
		std::ifstream ifs(mFilename + ".bin", std::ios::binary);
		if (ifs.fail()) {
			ifs.open(mFilename, std::ios::binary);
			if (ifs.fail()) {
				output.error("No such file or directory.");
				return;
			}
		}
		auto str = ifs2str(ifs);
		RBStream rs{ str };

		auto alpha = (mAlpha_isSet && mAlpha) ? 0 : (0xff << 24);

		for (int i = 0; i < 128; ++i)
			for (int j = 0; j < 128; ++j) {
				unsigned int val;
				rs.apply(val);
				mapd->setPixel(alpha | val, j, i);
			}
		mapd->setLocked();
		mapd->setOrigin(Vec3(1e9, 0., 1e9), 0, VanillaDimensions::Overworld, false, false, BlockPos((int)1e9, 0, (int)1e9));
		mapd->save(level->getLevelStorage());
		if (!mOutput_isSet || mOutput) output.success("Done!");
		else output.success();
		return;
	}

	static void setup(CommandRegistry* registry) {//注册部分(推荐做法)
		registry->registerCommand("map", "Customize the pixels on the map", CommandPermissionLevel::Any, { (CommandFlagValue)0 }, { (CommandFlagValue)0x80 });

		auto filename = RegisterCommandHelper::makeMandatory(&MapCommand::mFilename, "filename", &MapCommand::mFilename_isSet);
		auto alpha = RegisterCommandHelper::makeOptional(&MapCommand::mAlpha, "alpha", &MapCommand::mAlpha_isSet);
		auto output = RegisterCommandHelper::makeOptional(&MapCommand::mOutput, "output", &MapCommand::mOutput_isSet);
		
		registry->registerOverload<MapCommand>("map", filename, alpha, output);
	}
};

DBHelpers::Category const mapCategory = (DBHelpers::Category)6;

class MapOpCommand : public Command {

	enum class Operation {
		List,
		Info,
		Delete,
		Set
	} mOperation;
	bool mOperation_isSet;

	string mUUID;
	bool mUUID_isSet;

public:
	void execute(CommandOrigin const& ori, CommandOutput& output) const override {//执行部分
		if (mOperation == Operation::List) {
			string list;
			Global<DBStorage>->forEachKeyWithPrefix("map_", (DBHelpers::Category)6,
				[&](gsl::cstring_span<-1> key_left, gsl::cstring_span<-1> data) {
					list += key_left.data();
					list += ",";
				});
			list.pop_back();
			output.success(list);
			return;
		}

		if (mOperation == Operation::Info) {
			auto* player = ori.getPlayer();
			if (player == nullptr) {
				output.error("Not a player");
				return;
			}
			auto& item = player->getCarriedItem();
			ActorUniqueID id = MapItem::getMapId(item.getUserData());
			if (id == -1) {
				output.error("No map uuid");
				return;
			}
			//auto nbt = Global<DBStorage>->getCompoundTag(std::format("map_{}", (long long)id), mapCategory);
			//logger.info(nbt->toPrettySNBT());
			output.success(item.getUserData()->toPrettySNBT(true));
			return;
		}
		if (mOperation == Operation::Delete) {
			string mapKey;
			if (!mUUID_isSet) {
				auto* player = ori.getPlayer();
				if (player == nullptr) {
					output.error("Not a player");
					return;
				}
				auto& item = player->getCarriedItem();
				ActorUniqueID id = MapItem::getMapId(item.getUserData());
				if (id == -1) {
					output.error("No map uuid");
					return;
				}
				mapKey = std::format("map_{}", (long long)id);
				return;
			}
			else {
				mapKey = "map_" + mUUID;
			}
			DBStorage* db = Global<DBStorage>;
			if (db->hasKey(mapKey, mapCategory)) {
				db->deleteData(mapKey, mapCategory);
				output.success("Map data deleted");
			}
			else {
				output.error(std::format("Key {} not found", mapKey));
			}
			return;
		}

		if (mOperation == Operation::Set) {
			if (!mUUID_isSet) {
				output.error("No UUID");
				return;
			}
			auto* player = ori.getPlayer();
			if (player == nullptr) {
				output.error("Not a player");
				return;
			}
			auto& item = player->getCarriedItem();
			auto newItem = item.clone();
			try {
				newItem.getUserData()->getInt64Tag("map_uuid")->set(std::stoll(mUUID));
			}
			catch (std::exception &e) {
				output.error(e.what());
				return;
			}
			player->setCarriedItem(newItem);
			player->refreshInventory();
			output.success("UUID Modified");
		}
	}

	static void setup(CommandRegistry* registry) {//注册部分(推荐做法)
		registry->registerCommand("mapop", "Manage map data storage", CommandPermissionLevel::GameMasters, { (CommandFlagValue)0 }, { (CommandFlagValue)0x80 });

		registry->addEnum<Operation>("CustomMap_Action", {
			{"list", Operation::List},
			{"info", Operation::Info},
			{"del", Operation::Delete},
			{"set", Operation::Set}
			});

		auto action = RegisterCommandHelper::makeMandatory<CommandParameterDataType::ENUM>(&MapOpCommand::mOperation, "action", "CustomMap_Action", &MapOpCommand::mOperation_isSet);
		auto uuid = RegisterCommandHelper::makeOptional(&MapOpCommand::mUUID, "uuid", &MapOpCommand::mUUID_isSet);

		registry->registerOverload<MapOpCommand>("mapop", action, uuid);
	}
};

void exportAPIs() {
	RemoteCall::exportAs("CustomMap", "delMap", [](long long uuid) {
		string mapKey = std::format("map_{}", uuid);
		DBStorage* db = Global<DBStorage>;
		if (db->hasKey(mapKey, mapCategory)) {
			db->deleteData(mapKey, mapCategory);
			return true;
		}
		else {
			return false;
		}
		});

	RemoteCall::exportAs("CustomMap", "getMapList", []() {
		vector<long long> uuids;
		Global<DBStorage>->forEachKeyWithPrefix("map_", (DBHelpers::Category)6,
			[&](gsl::cstring_span<-1> key_left, gsl::cstring_span<-1> data) {
				try {
					uuids.push_back(std::stoll(key_left.data()));
				}
				catch (std::exception& e) {
					logger.error(e.what());
					return;
				}
			});
		return uuids;
		});

	RemoteCall::exportAs("CustomMap", "addMap", [](string filepath) {
		std::ifstream ifs(filepath, std::ios::binary);
		if (ifs.fail()) {
			return -1LL;
		}
		auto str = ifs2str(ifs);
		RBStream rs{ str };

		auto level = Global<ServerLevel>;
		auto id = level->getNewUniqueID();
		MapItemSavedData& mapd = level->_getMapDataManager().createMapSavedData(id, true);
		for (int i = 0; i < 128; ++i)
			for (int j = 0; j < 128; ++j) {
				unsigned int val;
				rs.apply(val);
				mapd.setPixel(val, j, i);
			}
		mapd.setLocked();
		mapd.setScale(4);
		mapd.setOrigin(Vec3(1e9, 0., 1e9), 0, VanillaDimensions::Overworld, false, false, BlockPos((int)1e9, 0, (int)1e9));
		mapd.save(level->getLevelStorage());
		return id.get();
		});
}

void PluginInit()
{
	ll::registerPlugin("CustomMap", "Customize the pixels on the map", ll::Version(1, 2, 6));
	Event::RegCmdEvent::subscribe([](Event::RegCmdEvent ev) { //注册指令事件
		MapCommand::setup(ev.mCommandRegistry);
		MapOpCommand::setup(ev.mCommandRegistry);
		return true;
	});
	exportAPIs();
	logger.info("CustomMap Loaded");
}