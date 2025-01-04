#pragma once
#include "datapacket.h"
#pragma pack(push)
#pragma pack(1)
struct IHideSpritePacket : ODataPacket
{
	char hide;
};

struct OHideSpritePacket : ODataPacket
{
	char hide;
	inline OHideSpritePacket(char hide) : hide(hide) 
	{
		flags = DataPacketFlag::broadcastForMap | DataPacketFlag::saveForPlayer;
		slot = DataPacketSlot::hidesprite;
	}
};

#pragma pack(pop)