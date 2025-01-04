#pragma once
#include "datapacket.h"
#include "../../game_actor.h"

#define actor_face_name_max_length (32)
#define actor_sprite_name_max_length (32)
#define actor_title_max_length (32)
#define actor_name_max_length (32)

#pragma pack(push)
#pragma pack(1)
struct IActorInfoPacket : public IDataPacket
{
	unsigned short battlerAnimation;
	unsigned short classId;
	unsigned short currentHp;
	unsigned short currentSp;
	unsigned short baseMaxHp;
	unsigned short baseMaxSp;
	unsigned char faceId;
	unsigned char level;
	unsigned char spriteId;
	unsigned char transparency;
	char title[actor_title_max_length];
	char spriteName[actor_sprite_name_max_length];
	char name[actor_name_max_length];
	char faceName[actor_face_name_max_length];
};

struct OActorInfoPacket : public ODataPacket
{
	unsigned short battlerAnimation;
	unsigned short classId;
	unsigned short currentHp;
	unsigned short currentSp;
	unsigned short baseMaxHp;
	unsigned short baseMaxSp;
	unsigned char faceId;
	unsigned char level;
	unsigned char spriteId;
	unsigned char transparency;
	char title[actor_title_max_length];
	char spriteName[actor_sprite_name_max_length];
	char name[actor_name_max_length];
	char faceName[actor_face_name_max_length];

	OActorInfoPacket(Game_Actor* actor, int partyIndex);
};
#pragma pack(pop)
/*
	TODO:
	Actor info packet is currently used for syncing any actor update with is not ideal
*/
