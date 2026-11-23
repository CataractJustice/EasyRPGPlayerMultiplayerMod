#include "datapacket.h"
#include <memory.h>
#include <string>
#define max_nametag_byte_length 32
#pragma pack(push)
#pragma pack(1)
struct INametagPacket : IDataPacket 
{
	char nametag[max_nametag_byte_length];
};

struct ONametagPacket : ODataPacket
{
	char nametag[max_nametag_byte_length];
	ONametagPacket(std::string name);
};

#pragma pack(pop)