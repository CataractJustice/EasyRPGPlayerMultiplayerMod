#pragma once
#include "datapacket.h"
#include "nametagpacket.h"
#include <string>
#define max_chat_message_length 512
#pragma pack(push)
#pragma pack(1)
struct OChatMsgPacket : ODataPacket
{
	char nametag[max_nametag_byte_length];
	char message[max_chat_message_length];
	OChatMsgPacket(const std::string& message, const std::string& nametag, unsigned char flags);
};


struct IChatMsgPacket : IDataPacket
{
	char nametag[max_nametag_byte_length];
	char message[max_chat_message_length];
};

#pragma pack(pop)