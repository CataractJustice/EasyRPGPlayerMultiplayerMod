#include "nametagpacket.h"
#define max_nametag_byte_length 32
ONametagPacket::ONametagPacket(std::string name) 
{
	memset(nametag, 0, max_nametag_byte_length);
	memcpy(nametag, name.c_str(), std::min(max_nametag_byte_length - 1, (int)name.length()));
	this->nametag[max_nametag_byte_length - 1] = 0;
	this->flags = DataPacketFlag::saveForPlayer | DataPacketFlag::broadcastForMap;
	this->slot = DataPacketSlot::nametag;
}