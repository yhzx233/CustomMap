#include "pch.h"
#include <llapi/mc/Level.hpp>
#include <llapi/LoggerAPI.h>

#include <llapi/mc/MapInfoRequestPacket.hpp>
#include <llapi/mc/MapItemSavedData.hpp>

#include <llapi/mc/Vec3.hpp>
#include <llapi/mc/VanillaDimensions.hpp>

extern Logger logger;

TInstanceHook(bool, "?replaceServerPixels@MapInfoRequestPacket@@QEBA_NAEAVMapItemSavedData@@@Z", MapInfoRequestPacket, MapItemSavedData &b) {
	if (b.isLocked()) {
		Vec3 farVec = Vec3(1e9, 0., 1e9);
		b.setOrigin(farVec, 0, VanillaDimensions::Overworld, false, false, BlockPos(farVec));
		b.save(Global<Level>->getLevelStorage());
		//logger.warn("Try fixing replacing locked map {}", b.getMapId());
		return false;
	}
	return original(this, b);
}