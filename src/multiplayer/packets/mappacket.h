#pragma once
#include "basepacket.h"
#pragma pack(push)
#pragma pack(1)
struct MapPacket : OutgoingPacket
{
	short room;
	MapPacket(int room)
	{
		this->type = 0;
		this->room = room;
	}
};

#pragma pack(pop)