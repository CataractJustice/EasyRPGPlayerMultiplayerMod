#pragma once
#include "datapacket.h"
#pragma pack(push)
#pragma pack(1)
struct IMoveSpeedPacket : IDataPacket
{
	unsigned char speed;
};

struct OMoveSpeedPacket : ODataPacket
{
	unsigned char speed;
	OMoveSpeedPacket(unsigned char speed);
};

#pragma pack(pop)