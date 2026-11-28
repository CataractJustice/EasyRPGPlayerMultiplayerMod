#pragma once
#include "basepacket.h"
#include <cstdint>
#pragma pack(push)
#pragma pack(1)
struct DataPacketFlag
{
	enum Flag {
	broadcastForMap = 1 << 0,//1
	broadcastForGame = 1 << 1,//2
	broadcastForServer = 1 << 2,//4
	saveForPlayer = 1 << 3,//8
	saveForMap = 1 << 4,//16
	saveForGame = 1 << 5,//32
	saveForServer = 1 << 6//64
	};
};

#define PIC_SLOTS 16
#define VAR_SLOTS 128
#define SWITCH_SLOTS 128
#define EVENT_SLOTS 128
#define ACTOR_SLOTS 8

#define SlotsRange(name, range) name##0, name##Last = name##0 + range - 1

struct DataPacketScope 
{
	enum Scope 
	{
		responce,
		personal,
		map,
		game,
		server,
		direct,
	};
};

struct DataPacketSlot
{
	enum Slot {
		//+ implemented
		//~ implemented with bugs
		//- not implemented
		//? needs testing
		position,//+
		facing,//+
		movespeed,//+
		sprite,//+
		hidesprite,//~
		sound,//+
		pflash,//+ player flash, continious by default
		mflash,//- flash screen for current map
		mtint,//- tint screen for current map
		weather,//-
		jump,//+
		chatmsg,//+
		nametag,//+
		system,//+
		spritebitmap,//-
		transparency,//?
		active,//-
		battlestart,//-
		SlotsRange(actorinfo, ACTOR_SLOTS),
		SlotsRange(pic, PIC_SLOTS), //-
		SlotsRange(var, VAR_SLOTS), //-
		SlotsRange(switch, SWITCH_SLOTS), //-
		SlotsRange(event, EVENT_SLOTS), //-
		lastSlot //
	};
};

struct IDataPacket : IncomingPacket
{
	unsigned char scope;
	uint64_t ownerId;
	unsigned int mapId;
	unsigned short slot;
};


struct ODataPacket : OutgoingPacket
{
	unsigned char flags;
	unsigned short slot;
	ODataPacket();
};

#pragma pack(pop)