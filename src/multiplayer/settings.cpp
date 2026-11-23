#include "settings.h"
#include "connection.h"
#include "packets/nametagpacket.h"
#include "init.h"

std::string clientUsername = "";
int currentMapId = -1;

const std::string& getClientUsername() 
{
	return clientUsername;
}
void setClientUsername(const std::string& username) 
{
	if(username.length() > max_nametag_byte_length - 1) return;
	clientUsername = username;
}

void setCurrentMapId(int map) 
{
	currentMapId = map;
}

int getCurrentMapId() 
{
	return currentMapId;
}

void sendAllSettings() 
{
	if(currentMapId >= 0) 
	{
		ChangeMap(currentMapId);
	}
	if(clientUsername.empty() == false) 
	{
		ONametagPacket nametagPacket(clientUsername);
		sendMessage((unsigned char*)&nametagPacket, sizeof(nametagPacket));
	}
}