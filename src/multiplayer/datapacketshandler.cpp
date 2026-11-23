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
#ifdef MULTIPLAYER_CHAT
#include "chat.h"
#endif
void HandleDataPacket(IDataPacket* packet) 
{
	if(packet->slot >= DataPacketSlot::lastSlot) 
	{
		Output::Warning("Received data with unknown slot {} with owner id {}", packet->slot, packet->ownerId);
	}
	std::shared_ptr<MultiplayerCharacter> dataOwnerCharacter = MultiplayerCharacter::GetPlayer(packet->ownerId);
	switch (packet->slot)
	{
	case DataPacketSlot::position:
		if(dataOwnerCharacter.get()) 
		{
			IPositionPacket* pos = (IPositionPacket*)packet;
			dataOwnerCharacter->MultiplayerMove(pos->x, pos->y);
		}
		break;
	case DataPacketSlot::movespeed:
	if(dataOwnerCharacter.get()) 
	{
		IMoveSpeedPacket* speed = (IMoveSpeedPacket*)packet;
		dataOwnerCharacter->SetMoveSpeed(speed->speed);
	}
	break;
	case DataPacketSlot::facing:
	if(dataOwnerCharacter.get()) 
	{
		IFacingPacket* facing = (IFacingPacket*)packet;
		dataOwnerCharacter->SetFacing(facing->facing);
	}
	break;
	case DataPacketSlot::sprite:
	if(dataOwnerCharacter.get()) 
	{
		ISpritePacket* sprite = (ISpritePacket*)packet;
		sprite->sheetname[max_sprite_packet_sheet_name_length - 1] = 0;
		dataOwnerCharacter->SetSpriteGraphic(std::string(sprite->sheetname), sprite->index);
	}
	break;
	case DataPacketSlot::sound:
	if(dataOwnerCharacter.get()) 
	{
		ISoundPacket* soundPacket = (ISoundPacket*)packet;
		lcf::rpg::Sound sound;
		soundPacket->soundname[max_sound_packet_sound_name_length - 1] = 0;
		sound.name = std::string(soundPacket->soundname);
		sound.volume = soundPacket->volume;
		sound.tempo = soundPacket->tempo;
		sound.balance = soundPacket->balance;
		dataOwnerCharacter->PlaySound(sound);
	}
	break;
	case DataPacketSlot::jump:
	if(dataOwnerCharacter.get()) 
	{
		IJumpPacket* jump = (IJumpPacket*)packet;
		dataOwnerCharacter->Jump(jump->x, jump->y);
	}
	break;
	
	case DataPacketSlot::chatmsg:
	{
		#ifdef MULTIPLAYER_CHAT
		IChatMsgPacket* msg = (IChatMsgPacket*)packet;
		if(Chat::HasInstance())
			Chat::Instance().PushMessage(msg);
		#endif
	}
	break;
	case DataPacketSlot::hidesprite:
	if(dataOwnerCharacter.get()) 
	{
		IHideSpritePacket* hide = (IHideSpritePacket*)packet;
		dataOwnerCharacter->SetSpriteHidden(hide->hide);
	}
	break;

	case DataPacketSlot::transparency:
	if(dataOwnerCharacter.get()) 
	{
		ITransparencyPacket* transparency = (ITransparencyPacket*)packet;
		dataOwnerCharacter->SetTransparency(transparency->transparency);
	}
	break;

	case DataPacketSlot::system:
	if(dataOwnerCharacter.get()) 
	{
		ISystemPacket* system = (ISystemPacket*)packet;
		dataOwnerCharacter->SetSystem(system->systemname);
	}
	break;

	case DataPacketSlot::pflash:
	if(dataOwnerCharacter.get()) 
	{
		IFlashPacket* flash = (IFlashPacket*)packet;
		flash->scope;
		dataOwnerCharacter->SetFlash(flash->r, flash->g, flash->b, flash->power, flash->frames);
	}
	
	default:
		if(dataOwnerCharacter.get()) 
		{
			if(packet->slot >= DataPacketSlot::actorinfo0 && packet->slot <= DataPacketSlot::actorinfoLast) 
			{
				int actorIndex = packet->slot - DataPacketSlot::actorinfo0;
				
				dataOwnerCharacter->UpdateActor((IActorInfoPacket*)packet, actorIndex);
			}
		}
		break;
	}
}