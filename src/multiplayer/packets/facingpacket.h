#pragma once
#include "datapacket.h"
#pragma pack(push)
#pragma pack(1)
struct IFacingPacket : IDataPacket
{
	unsigned char facing;
};

struct OFacingPacket : ODataPacket
{
	unsigned char facing;
	OFacingPacket(unsigned char facing);
};

#pragma pack(pop)