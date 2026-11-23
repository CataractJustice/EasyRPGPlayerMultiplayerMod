#include "nametagpacket.h"
#define max_nametag_byte_length 32
ONametagPacket::ONametagPacket(std::string name) 
{
	memset(nametag, 0, max_nametag_byte_length);
	memcpy(nametag, name.c_str(), std::min(max_nametag_byte_length, (int)name.length()));
	this->flags = DataPacketFlag::saveForPlayer | DataPacketFlag::broadcastForServer;
	this->slot = DataPacketSlot::nametag;
}