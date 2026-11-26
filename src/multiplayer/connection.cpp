#include <iostream>
#include "connection.h"
#include "output.h"
#include "packets/basepacket.h"
#include "multiplayercharacter.h"
#include "packets/connectpacket.h"
#include "packets/inmappacket.h"
#include "packets/disconnectpacket.h"
#include "datapacketshandler.h"
#include "init.h"
#include "settings.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/websocket.h>
EMSCRIPTEN_WEBSOCKET_T emsocket;

EM_BOOL onEmOpen(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData) {
	onConnect();
	return EM_TRUE;
}

EM_BOOL onEmClose(int eventType, const EmscriptenWebSocketCloseEvent *websocketEvent, void *userData) {
	emscripten_websocket_deinitialize();
	return EM_TRUE;
}


EM_BOOL onEmMessage(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData) {
	onMessage(websocketEvent->data, websocketEvent->numBytes);
	return EM_TRUE;
}

#else
#include <queue>
#include <mutex>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>

ix::WebSocket* webSocketPtr;
std::queue<std::pair<unsigned char*, unsigned int>> packetQ;
std::mutex packetQLock;
#endif

void onMessageAsync(unsigned char* data, unsigned int byteLength) 
{
	#ifndef __EMSCRIPTEN__
	packetQLock.lock();
	unsigned char* msgcopy = new unsigned char[byteLength];
	memcpy(msgcopy, data, byteLength);
	packetQ.push({msgcopy, byteLength});
	packetQLock.unlock();
	#endif
}

void Connect(std::string url, std::string game, Credentials credentials) 
{
	std::string connectionURL = url + "?game=" + game;
	#ifdef __EMSCRIPTEN__
	//create emscripten WS//
	EmscriptenWebSocketCreateAttributes ws_attrs = {
		connectionURL.c_str(),
		"binary",
		EM_TRUE
	};

	emsocket = emscripten_websocket_new(&ws_attrs);
	emscripten_websocket_set_onopen_callback(emsocket, NULL, onEmOpen);
	emscripten_websocket_set_onclose_callback(emsocket, NULL, onEmClose);
	emscripten_websocket_set_onmessage_callback(emsocket, NULL, onEmMessage);
	#else
	ix::initNetSystem();
	webSocketPtr = new ix::WebSocket();
	ix::WebSocket& webSocket = *webSocketPtr;
	webSocket.setUrl(connectionURL.c_str());
	webSocket.setHandshakeTimeout(5);
	webSocket.disablePerMessageDeflate();
	Output::Debug("Multiplayer Setting WebSocket callbacks.");
	webSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg)
		{
			if (msg->type == ix::WebSocketMessageType::Message)
			{
				onMessageAsync((unsigned char*)(msg->str.c_str()), msg->str.length());
			}
			else if (msg->type == ix::WebSocketMessageType::Open)
			{
				Output::Debug("Multiplayer WebSocket open.");
			}
			else if (msg->type == ix::WebSocketMessageType::Error)
			{
				if(msg->errorInfo.http_status)
					Output::Warning("IXWebSocket error: {}", msg->errorInfo.reason);
				else
					Output::Warning("{}", "Unknow IXWebSocket error");
			} 
			else if(msg->type == ix::WebSocketMessageType::Close) 
			{
				Output::Debug("Multiplayer WebSocket connection closed: {}.", msg->closeInfo.reason);
			}
		}
	);

	Output::Debug("Multiplayer Starting WebSocket.");
	webSocket.start();
	#endif
}

int reconnectTimeout = 200;

void ConnectionUpdate() 
{
	#ifndef __EMSCRIPTEN__
	while (true) 
	{
		std::pair<unsigned char*, unsigned int> packetPair = {0,0};
		packetQLock.lock();
		if(packetQ.size() == 0) 
		{
			packetQLock.unlock();
			break;
		}
		packetPair = packetQ.front();
		packetQ.pop();
		packetQLock.unlock();

		onMessage(packetPair.first, packetPair.second);
		delete[] packetPair.first;
	}
	#else
	unsigned short readyState = 0;

	if (emsocket != 0 &&
		emscripten_websocket_get_ready_state(emsocket, &readyState)
			== EMSCRIPTEN_RESULT_SUCCESS &&
		readyState == 1) 
	{
		EM_ASM({
			window.parent.EasyRPGMultiplayer.setConnection(true);
		});
	}
	else 
	{
		reconnectTimeout--;
		if(reconnectTimeout <= 0) 
		{
			Connect(getServerURL(), getGameName(), Credentials());
			reconnectTimeout = 300;
			EM_ASM({
				window.parent.EasyRPGMultiplayer.setConnection(false);
			});
		}
	}
	#endif
}

void onConnect() 
{
	sendAllSettings();
}

void sendMessage(unsigned char* data, unsigned int byteLength) 
{
	#ifdef __EMSCRIPTEN__
	unsigned short readyState = 0;

	if (emsocket != 0 &&
		emscripten_websocket_get_ready_state(emsocket, &readyState)
			== EMSCRIPTEN_RESULT_SUCCESS &&
		readyState == 1) {

		emscripten_websocket_send_binary(emsocket, data, byteLength);
	}
	#else
	webSocketPtr->sendBinary(ix::IXWebSocketSendData((char*)data, byteLength));
	#endif
}

void onMessage(unsigned char* data, unsigned int byteLength) 
{
	IncomingPacket packet;
	memcpy(&packet, data, sizeof(packet));
	switch (packet.type)
	{
	case IPacketType::connect:
	{
		if(sizeof(ConnectPacket) != byteLength) return;
		ConnectPacket connectPacket;
		memcpy(&connectPacket, data, byteLength);
		MultiplayerCharacter::MakeCharacter(connectPacket.id);
	}
		break;
	case IPacketType::inmap: 
	{
		if(sizeof(InMapPacket) != byteLength) return;
		InMapPacket inMapPacket;
		memcpy(&inMapPacket, data, byteLength);
		MultiplayerCharacter::MakeCharacter(inMapPacket.id);
	}
	break;

	case IPacketType::disconnect:
	{
		if(sizeof(DisconnectPacket) != byteLength) return;
		DisconnectPacket disconnectPacket;
		memcpy(&disconnectPacket, data, byteLength);
		MultiplayerCharacter::RemovePlayer(disconnectPacket.id);
	}
	break;

	case IPacketType::mdata:
	{
		if(sizeof(IDataPacket) > byteLength) return;
		Output::Debug("Data packet received");
		HandleDataPacket(data, byteLength);
	}
	break;
	
	case IPacketType::id: 
	{
		//Multiplayer to-do: save id if it ever going to be needed.
	}
	break;
	}
}

void ConnectionClose()
{
	#ifdef __EMSCRIPTEN__
	#else
	webSocketPtr->stop();
	#endif
}