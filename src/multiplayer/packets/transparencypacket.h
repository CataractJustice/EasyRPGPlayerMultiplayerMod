#pragma once
#include "datapacket.h"
#include <string>

#define max_sprite_packet_sheet_name_length 64
#pragma pack(push)
#pragma pack(1)
struct ITransparencyPacket : IDataPacket
{
	unsigned char transparency;
};

struct OTransparencyPacket : ODataPacket
{
	unsigned char transparency;
	OTransparencyPacket(unsigned char transparency) 
	{
		this->transparency = transparency;
		this->slot = DataPacketSlot::transparency;
		this->flags = DataPacketFlag::saveForPlayer | DataPacketFlag::broadcastForMap;
	}
};
#pragma pack(pop)