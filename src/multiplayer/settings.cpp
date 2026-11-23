#include "settings.h"
#include "connection.h"
#include "packets/nametagpacket.h"

std::string clientUsername = "";

const std::string& getClientUsername() 
{
	return clientUsername;
}
void setClientUsername(const std::string& username) 
{
	if(username.length() > max_nametag_byte_length - 1) return;
	clientUsername = username;
}

void sendAllSettings() 
{
	ONametagPacket nametagPacket(clientUsername);
    sendMessage((unsigned char*)&nametagPacket, sizeof(nametagPacket));
}