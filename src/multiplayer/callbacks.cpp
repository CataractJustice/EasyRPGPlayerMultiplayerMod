#include <string>
#include "connection.h"
#include "packets/nametagpacket.h"
#include "packets/chatmsgpacket.h"
#include "settings.h"
#include "init.h"
#include "multiplayercharacter.h"
#include "output.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
void sendName(const std::string& username) {
	if(username.length() > max_nametag_byte_length - 1) return;
	ONametagPacket nametagPacket(username);
	sendMessage((unsigned char*)&nametagPacket, sizeof(nametagPacket));
	setClientUsername(username);
}

void sendChatMessage(const std::string& text, const std::string nametag, bool isGlobal) {
	if(text.length() > max_chat_message_length - 1) return;
	unsigned char flags = isGlobal ? DataPacketFlag::broadcastForGame : DataPacketFlag::broadcastForMap;
	OChatMsgPacket chatPacket(text, nametag, flags);
    sendMessage((unsigned char*)&chatPacket, sizeof(chatPacket));
	if(Settings::chatBubbles) CreateOwnTextBubble(text);
}

void setShowOwnNametag(bool show) 
{
	setShouldDisplayOwnNametag(show);
}

int getMapPlayerCount() 
{
	return MultiplayerCharacter::GetPlayerCount();
}

void setUserNametagById(const std::string& userId64, const std::string& username) 
{
	Output::Debug("Username loaded");
	uint64_t userId = std::stoull(userId64);
	MultiplayerCharacter::characters[userId]->SetNametag(username);
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("sendName", &sendName);
    emscripten::function("sendChatMessage", &sendChatMessage);
	emscripten::function("setShowOwnNametag", &setShowOwnNametag);
	emscripten::function("setOwnSprite", &setCustomSprite);
	emscripten::function("unsetOwnSprite", &restoreRealSprite);
	emscripten::function("getMapPlayerCount", &getMapPlayerCount);
	emscripten::function("setUserNametagById", &setUserNametagById);
}
#endif