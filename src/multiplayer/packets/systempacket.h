#pragma once
#include "datapacket.h"
#include <string>
#include <memory.h>

#define max_system_packet_system_name_length 64
#pragma pack(push)
#pragma pack(1)
struct ISystemPacket : IDataPacket
{
	char systemname[max_system_packet_system_name_length];
};

struct OSystemPacket : ODataPacket
{
	char systemname[max_system_packet_system_name_length];
	OSystemPacket(std::string systemname) 
	{
		this->flags = DataPacketFlag::broadcastForMap | DataPacketFlag::saveForPlayer;
		memset(this->systemname, 0, max_system_packet_system_name_length);
		memcpy(this->systemname, systemname.c_str(), std::min((int)systemname.length(), max_system_packet_system_name_length));
		this->systemname[max_system_packet_system_name_length - 1] = 0;
		this->slot = DataPacketSlot::system;
	} 
};
#pragma pack(pop)