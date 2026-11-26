#pragma once
#include <string>
std::string getGameName();
std::string getServerURL();
void MultiplayerStart();
void MultiplayerUpdate();
void MultiplayerClose();
void ChangeMap(int map);
void CreateOwnTextBubble(const std::string& text);
void UpdateOwnTextBubbles();