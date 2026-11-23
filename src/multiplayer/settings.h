#pragma once
#include <string>

const std::string& getClientUsername();
void setClientUsername(const std::string& name);

void sendAllSettings();

void setCurrentMapId(int map);

int getCurrentMapId();