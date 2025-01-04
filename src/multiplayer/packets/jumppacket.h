#include "datapacket.h"
#pragma pack(push)
#pragma pack(1)
struct IJumpPacket : IDataPacket 
{
	unsigned short x;
	unsigned short y;
};

struct OJumpPacket : ODataPacket
{
	unsigned short x;
	unsigned short y;
	OJumpPacket(unsigned short x, unsigned short y) 
	{
		this->x = x;
		this->y = y;
		this->slot = DataPacketSlot::jump;
		this->flags = DataPacketFlag::broadcastForMap;
	}
};

#pragma pack(pop)