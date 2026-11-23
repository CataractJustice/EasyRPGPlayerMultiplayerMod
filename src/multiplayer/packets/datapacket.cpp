#include "datapacket.h"

ODataPacket::ODataPacket()
{
	this->type = 1;
	this->ownerId = 0;
	this->scope = DataPacketScope::map;
}