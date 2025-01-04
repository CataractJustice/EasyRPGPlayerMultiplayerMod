#pragma once
#include "datapacket.h"
#pragma pack(push)
#pragma pack(1)
struct IFlashPacket : IDataPacket
{
	unsigned char r, g, b, power, frames;
};

struct OFlashPacket : ODataPacket
{
	unsigned char r, g, b, power, frames;
	OFlashPacket(unsigned char r, unsigned char g, unsigned char b, unsigned char power, unsigned char frames) 
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->power = power;
		this->frames = frames;

		this->slot = DataPacketSlot::pflash;
		this->flags = DataPacketFlag::broadcastForMap | DataPacketFlag::saveForPlayer;
	}
};

#pragma pack(pop)