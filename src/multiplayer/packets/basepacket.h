#pragma once

#pragma pack(push)
#pragma pack(1)
struct IPacketType
{
	enum Type
	{
		connect,
		inmap,
		disconnect,
		mdata,
		id
	};
};

struct OutgoingPacket
{
	unsigned char type;
};

struct IncomingPacket
{
	unsigned char type;
};
#pragma pack(pop)