#pragma once
#include <string>

// Runtime multiplayer state and logic for the local player (username, current
// map, custom sprite, own nametag, outgoing packets). This is kept separate
// from the user configurable settings declared in settings.h.

const std::string& getClientUsername();
void setClientUsername(const std::string& name);

void setCurrentMapId(int map);
int getCurrentMapId();

bool getShouldDisplayOwnNametag();
void setShouldDisplayOwnNametag(bool value);
bool getShouldDisplayNametags();
void setShouldDisplayNametags(bool value);
void updateOwnNametag();
void updateOwnNametagBitmap();

bool hasCustomSprite();
void setCustomSprite(const std::string& sheet, int index);
void restoreRealSprite();
void storeRealSprite(const std::string& sheet, int index);
void getCustomSprite(std::string& sheet, int& index);

void sendAllSettings();