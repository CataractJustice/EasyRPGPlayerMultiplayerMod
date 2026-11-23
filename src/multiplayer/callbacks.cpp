#include <string>
#include "connection.h"
#include "packets/nametagpacket.h"
#include "packets/chatmsgpacket.h"
#include "settings.h"

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
	OChatMsgPacket chatPacket(text, nametag, flags, isGlobal);
    sendMessage((unsigned char*)&chatPacket, sizeof(chatPacket));
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("sendName", &sendName);
    emscripten::function("sendChatMessage", &sendChatMessage);
}
#endif