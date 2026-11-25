#pragma once
#include <string>

void sendAllSettings();

const std::string& getClientUsername();
void setClientUsername(const std::string& name);

void setCurrentMapId(int map);
int getCurrentMapId();

bool getShouldDisplayOwnNametag();
void setShouldDisplayOwnNametag(bool value);
void updateOwnNametag();
void updateOwnNametagBitmap();