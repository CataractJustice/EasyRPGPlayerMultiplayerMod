#include <cstring>
#include "output.h"
#include "multiplayercharacter.h"
#include "datapacketshandler.h"
#include "packets/positionpacket.h"
#include "packets/movespeedpacket.h"
#include "packets/facingpacket.h"
#include "packets/spritepacket.h"
#include "packets/jumppacket.h"
#include "packets/soundpacket.h"
#include "packets/spritebitmappacket.h"
#include "packets/chatmsgpacket.h"
#include "packets/hidespritepacket.h"
#include "packets/transparencypacket.h"
#include "packets/flashpacket.h"
#include "packets/systempacket.h"
#include "packets/actorinfopacket.h"
#include "packets/nametagpacket.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#ifdef INTEGRATED_CHAT
#include "chat.h"
#endif

void HandleDataPacket(unsigned char* packetData, int byteLength) 
{
	if(sizeof(IDataPacket) > byteLength)
		return;
	IDataPacket packetHeader;
	memcpy(&packetHeader, packetData, sizeof(IDataPacket));
	Output::Debug("New packet data with byteLength="+std::to_string(byteLength) + " and slot " + std::to_string(packetHeader.slot) + " and ownerId="+std::to_string(packetHeader.ownerId));
	if(packetHeader.slot >= DataPacketSlot::lastSlot) 
	{
		Output::Warning("Received data with unknown slot {} with owner id {}", packetHeader.slot, packetHeader.ownerId);
	}
	std::shared_ptr<MultiplayerCharacter> dataOwnerCharacter = MultiplayerCharacter::GetPlayer(packetHeader.ownerId);
	switch (packetHeader.slot)
	{
	case DataPacketSlot::position:
		if(dataOwnerCharacter.get()) 
		{
			if(sizeof(IPositionPacket) != byteLength)
				return;
			IPositionPacket pos;
			memcpy(&pos, packetData, sizeof(IPositionPacket));
			dataOwnerCharacter->MultiplayerMove(pos.x, pos.y);
		}
	break;
	case DataPacketSlot::movespeed:
	if(dataOwnerCharacter.get()) 
	{
		if(sizeof(IMoveSpeedPacket) != byteLength)
			return;
		IMoveSpeedPacket speed;
		memcpy(&speed, packetData, sizeof(IMoveSpeedPacket));
		dataOwnerCharacter->SetMoveSpeed(speed.speed);
	}
	break;
	case DataPacketSlot::facing:
	if(dataOwnerCharacter.get()) 
	{
		if(sizeof(IFacingPacket) != byteLength)
			return;
		IFacingPacket facing;
		memcpy(&facing, packetData, sizeof(IFacingPacket));
		dataOwnerCharacter->SetFacing(facing.facing);
	}
	break;
	case DataPacketSlot::sprite:
	if(dataOwnerCharacter.get()) 
	{
		if(sizeof(ISpritePacket) != byteLength)
			return;
		ISpritePacket sprite;
		memcpy(&sprite, packetData, sizeof(ISpritePacket));
		sprite.sheetname[max_sprite_packet_sheet_name_length - 1] = 0;
		dataOwnerCharacter->SetSpriteGraphic(std::string(sprite.sheetname), sprite.index);
	}
	break;
	case DataPacketSlot::sound:
	if(dataOwnerCharacter.get()) 
	{
		if(sizeof(ISoundPacket) != byteLength)
			return;
		ISoundPacket soundPacket;
		memcpy(&soundPacket, packetData, sizeof(ISoundPacket));
		lcf::rpg::Sound sound;
		soundPacket.soundname[max_sound_packet_sound_name_length - 1] = 0;
		sound.name = std::string(soundPacket.soundname);
		sound.volume = soundPacket.volume;
		sound.tempo = soundPacket.tempo;
		sound.balance = soundPacket.balance;
		dataOwnerCharacter->PlaySound(sound);
	}
	break;
	case DataPacketSlot::jump:
	if(dataOwnerCharacter.get()) 
	{
		if(sizeof(IJumpPacket) != byteLength)
			return;
		IJumpPacket jump;
		memcpy(&jump, packetData, sizeof(IJumpPacket));
		dataOwnerCharacter->Jump(jump.x, jump.y);
	}
	break;

	case DataPacketSlot::hidesprite:
	if(dataOwnerCharacter.get()) 
	{
		if(sizeof(IHideSpritePacket) != byteLength)
			return;
		IHideSpritePacket hide;
		memcpy(&hide, packetData, sizeof(IHideSpritePacket));
		dataOwnerCharacter->SetSpriteHidden(hide.hide);
	}
	break;

	case DataPacketSlot::transparency:
	if(dataOwnerCharacter.get()) 
	{
		if(sizeof(ITransparencyPacket) != byteLength)
			return;
		ITransparencyPacket transparency;
		memcpy(&transparency, packetData, sizeof(ITransparencyPacket));
		dataOwnerCharacter->SetTransparency(transparency.transparency);
	}
	break;

	case DataPacketSlot::system:
	if(dataOwnerCharacter.get()) 
	{
		if(sizeof(ISystemPacket) != byteLength)
			return;
		ISystemPacket system;
		memcpy(&system, packetData, sizeof(ISystemPacket));
		dataOwnerCharacter->SetSystem(std::string(system.systemname, strnlen(system.systemname, max_system_packet_system_name_length)));
	}
	break;

	case DataPacketSlot::pflash:
	if(dataOwnerCharacter.get()) 
	{
		if(sizeof(IFlashPacket) != byteLength)
			return;
		IFlashPacket flash;
		memcpy(&flash, packetData, sizeof(IFlashPacket));
		dataOwnerCharacter->SetFlash(flash.r, flash.g, flash.b, flash.power, flash.frames);
	}
	break;

	case DataPacketSlot::nametag: 
	if(dataOwnerCharacter.get()) 
	{
		if(sizeof(INametagPacket) != byteLength)
			return;
		INametagPacket nametag;
		memcpy(&nametag, packetData, sizeof(INametagPacket));
		dataOwnerCharacter->SetNametag(std::string(nametag.nametag, strnlen(nametag.nametag, max_nametag_byte_length)));
	}
	break;

	case DataPacketSlot::chatmsg:
	{
		IChatMsgPacket msg;
		memcpy(&msg, packetData, sizeof(IChatMsgPacket));
		#ifdef __EMSCRIPTEN__
		#ifdef INTEGRATED_CHAT
		if(sizeof(IChatMsgPacket) != byteLength)
			return;
		if(Chat::HasInstance())
			Chat::Instance().PushMessage(&msg);
		#else
		msg.nametag[max_nametag_byte_length - 1] = 0;
		msg.message[max_chat_message_length - 1] = 0;
		std::string author = dataOwnerCharacter.get() ? dataOwnerCharacter->GetNametag() : std::string(msg.nametag);
		EM_ASM({
			window.parent.EasyRPGMultiplayer.receiveMessage({
				author: UTF8ToString($0),
				message: UTF8ToString($1)
			});
		}, author.c_str(), msg.message);
		#endif
		#endif
	}
	break;
	
	default:
	#ifdef MULTIPLAYER_PARTIES
		if(dataOwnerCharacter.get()) 
		{
			if(packetHeader.slot >= DataPacketSlot::actorinfo0 && packetHeader.slot <= DataPacketSlot::actorinfoLast) 
			{
				int actorIndex = packetHeader.slot - DataPacketSlot::actorinfo0;
				if(sizeof(IActorInfoPacket) != byteLength)
					return;
				IActorInfoPacket actorInfo;
				memcpy(&actorInfo, packetData, sizeof(IActorInfoPacket));
				dataOwnerCharacter->UpdateActor(&actorInfo, actorIndex);
			}
		}
		break;
	#endif
	break;
	}
}