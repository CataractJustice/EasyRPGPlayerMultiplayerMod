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

void sendChatMessage(const std::string& text, const std::string nametag) {
	if(text.length() > max_chat_message_length - 1) return;
	OChatMsgPacket chatPacket(text, nametag, DataPacketFlag::broadcastForGame);
    sendMessage((unsigned char*)&chatPacket, sizeof(chatPacket));
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("sendName", &sendName);
    emscripten::function("sendChatMessage", &sendChatMessage);
}
#endif