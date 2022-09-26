#include "pch.h"
#include <llapi/EventAPI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/BlockInstance.hpp>
#include <llapi/mc/Block.hpp>
#include <llapi/mc/BlockSource.hpp>
#include <llapi/mc/Actor.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/ItemStack.hpp>
#include <llapi/LLAPI.h>

#include <llapi/RegCommandAPI.h>
#include <llapi/mc/ServerPlayer.hpp>
#include <llapi/mc/ActorUniqueID.hpp>
#include <llapi/mc/MapItemSavedData.hpp>
#include <llapi/mc/MapItem.hpp>

#include <iostream>
#include <fstream>

#include <llapi/Utils/Bstream.h>

Logger logger("CustomMap");

static inline string ifs2str(std::ifstream& ifs) {
	return { (std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>() };
}

bool oncmd_map(CommandOrigin const& ori, CommandOutput& outp, string const& name) {
	ServerPlayer* player = ori.getPlayer();
	if (!player->isPlayer()) {
		outp.addMessage("Not a player");
		return false;
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
	if (!mapd) { outp.error("Not holding a filled map."); return false; }
	mapd->setLocked();
	std::ifstream ifs(name + ".bin", std::ios::binary);
	if (ifs.fail()) {
		ifs.open(name, std::ios::binary);
		if (ifs.fail()) {
			outp.error("No such file or directory.");
			return false;
		}
	}
	auto str = ifs2str(ifs);
	RBStream rs{ str };
	for (int i = 0; i < 128; ++i)
		for (int j = 0; j < 128; ++j) {
			unsigned int val;
			rs.apply(val);
			mapd->setPixel(0xff000000 | val, j, i);
		}
	mapd->save(level->getLevelStorage());
	outp.success("Done!");
	return true;
}

class MapCommand : public Command {
	string filename;
	bool filename_isSet;
public:
	void execute(CommandOrigin const& ori, CommandOutput& output) const override {//ִ�в���
		oncmd_map(ori, output, filename);
	}

	static void setup(CommandRegistry* registry) {//ע�Ჿ��(�Ƽ�����)
		registry->registerCommand("map", "Customize the pixels on the map", CommandPermissionLevel::Any, { (CommandFlagValue)0 }, { (CommandFlagValue)0x80 });
		registry->registerOverload<MapCommand>(
			"map",
			RegisterCommandHelper::makeMandatory(&MapCommand::filename, "filename", &MapCommand::filename_isSet));
	}
};

void PluginInit()
{
	ll::registerPlugin("CustomMap", "Customize the pixels on the map", ll::Version(1, 1, 6));
	logger.info("CustomMap Loaded");
	Event::RegCmdEvent::subscribe([](Event::RegCmdEvent ev) { //ע��ָ���¼�
		MapCommand::setup(ev.mCommandRegistry);
		return true;
		});
}