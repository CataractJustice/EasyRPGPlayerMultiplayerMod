#pragma once
#include "datapacket.h"
#pragma pack(push)
#pragma pack(1)
struct IPositionPacket : IDataPacket
{
	unsigned short x;
	unsigned short y;
};

struct OPositionPacket : ODataPacket
{
	unsigned short x;
	unsigned short y;
	OPositionPacket(unsigned short x, unsigned short y) 
	{
		flags = DataPacketFlag::saveForPlayer | DataPacketFlag::broadcastForMap;
		slot = DataPacketSlot::position;
		this->x = x;
		this->y = y;
	}
};
#pragma pack(pop)