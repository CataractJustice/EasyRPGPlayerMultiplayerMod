#include "actorinfopacket.h"
#include "main_data.h"

OActorInfoPacket::OActorInfoPacket(Game_Actor* actor, int partyIndex) 
{
	this->slot = DataPacketSlot::actorinfo0 + partyIndex;
	this->flags = DataPacketFlag::saveForPlayer | DataPacketFlag::broadcastForMap;

	this->battlerAnimation = actor->GetBattleAnimationId();
	this->currentHp = actor->GetHp();
	this->currentSp = actor->GetSp();
	this->faceId = actor->GetFaceIndex();
	this->level = actor->GetLevel();
	this->spriteId = actor->GetSpriteIndex();
	this->transparency = actor->GetSpriteTransparency();
	this->baseMaxHp = actor->GetBaseMaxHp();
	this->baseMaxSp = actor->GetBaseMaxSp();

	
	memset(title, 		0, actor_title_max_length);
	memset(faceName, 	0, actor_face_name_max_length);
	memset(spriteName, 0, actor_sprite_name_max_length);
	memset(name, 		0, actor_name_max_length);

	memcpy(title, 		actor->GetTitle().data(), 		actor->GetTitle().length());
	memcpy(faceName, 	actor->GetFaceName().data(), 	actor->GetFaceName().length());
	memcpy(spriteName, actor->GetSpriteName().data(), 	actor->GetSpriteName().length());
	memcpy(name, 		actor->GetName().data(), 		actor->GetName().length());

	title[actor_title_max_length - 1] 		= actor_title_max_length;
	faceName[actor_title_max_length - 1] 	= actor_face_name_max_length;
	spriteName[actor_title_max_length - 1] = actor_sprite_name_max_length;
	name[actor_title_max_length - 1] 		= actor_name_max_length;
}