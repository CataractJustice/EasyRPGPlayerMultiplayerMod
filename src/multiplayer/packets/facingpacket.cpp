#include "facingpacket.h"

OFacingPacket::OFacingPacket(unsigned char facing) 
{
	this->flags = DataPacketFlag::broadcastForMap | DataPacketFlag::saveForPlayer;
	this->facing = facing;
	this->slot = DataPacketSlot::facing;
}