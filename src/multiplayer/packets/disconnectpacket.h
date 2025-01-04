#pragma once
#include "basepacket.h"
#pragma pack(push)
#pragma pack(1)
struct DisconnectPacket : IncomingPacket 
{
	unsigned int id;
};
#pragma pack(pop)