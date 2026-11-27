#pragma once
#include "basepacket.h"
#include <cstdint>
#pragma pack(push)
#pragma pack(1)
struct InMapPacket : IncomingPacket
{
    uint64_t accountId;
};
#pragma pack(pop)