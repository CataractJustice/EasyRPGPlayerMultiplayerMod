#include "datapacket.h"
#include "memory_management.h"

#define max_packed_sprite_bitmap_size 2048
#pragma pack(push)
#pragma pack(1)
struct ISpriteBitmapPacket : IDataPacket 
{
	unsigned char packedBitmap[max_packed_sprite_bitmap_size];
};

struct OSpriteBitmapPacket : ODataPacket 
{
	unsigned char packedBitmap[max_packed_sprite_bitmap_size];

	OSpriteBitmapPacket(BitmapRef& bitmap);
};

#pragma pack(pop)