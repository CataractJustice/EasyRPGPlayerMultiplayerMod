
#pragma once
#include <string>
#include <cstdint>

struct Credentials
{
    std::string authToken;
    uint64_t accountId = 0;
    std::string accountType;
    std::string username;
    std::string guestUuid;
};
