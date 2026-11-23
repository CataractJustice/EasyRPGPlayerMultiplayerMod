#include "chatmsgpacket.h"
#include "memory.h"

OChatMsgPacket::OChatMsgPacket(const std::string& message, const std::string& nametag, unsigned char flags) 
{
	memset(this->message, 0, sizeof(this->message));
	memset(this->nametag, 0, sizeof(this->nametag));
	memcpy(this->message, message.c_str(), std::min((int)message.length(), max_chat_message_length));
	memcpy(this->nametag, nametag.c_str(), std::min((int)nametag.length(), max_nametag_byte_length));
	this->message[max_chat_message_length - 1] = 0;
	this->nametag[max_nametag_byte_length - 1] = 0;
	this->flags = flags;
	this->slot = DataPacketSlot::chatmsg;
}