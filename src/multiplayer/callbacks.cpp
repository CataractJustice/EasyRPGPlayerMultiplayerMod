#include <string>
#include "connection.h"
#include "packets/nametagpacket.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
void sendName(std::string username) {
	if(username.length() > max_nametag_byte_length - 1) return;
	ONametagPacket nametagPacket(username);
    sendMessage((unsigned char*)&nametagPacket, sizeof(nametagPacket));
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("sendName", &sendName);
}
#endif