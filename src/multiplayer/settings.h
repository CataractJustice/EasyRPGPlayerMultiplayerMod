#pragma once
#include <string>

namespace Settings 
{
	extern bool sfxSync;
	extern int sfxVolume;
	extern int sfxDistance;
	extern bool npcSync;
	extern bool eventSync;
	extern bool npcSpriteSync;
	extern bool npcActivitySync;
	extern bool drawOwnNametag;
	extern bool drawOtherNametags;
	extern bool nametagSystemSync;
	extern bool chatBubbles;
	extern int chatBubbleFadeFrames;
	extern int chatBubbleFramesBeforeFade;
	extern int chatBubbleFloatSpeed;
}

void sendAllSettings();

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