#include "init.h"
#include "connection.h"
#include "output.h"
#include "packets/mappacket.h"
#include "multiplayer/multiplayercharacter.h"
#include <emscripten.h>
#include "settings.h"

#ifdef __EMSCRIPTEN__
std::string getGameName()
{
	const char* result = emscripten_run_script_string(
		"new URLSearchParams(window.location.search).get('game') || ''"
	);

	return result ? result : "";
}
std::string getServerURL() 
{
	const char* result = emscripten_run_script_string(
		"window.parent.getServerURL()"
	);
	return result ? result : "";
}
#else
std::string getGameName() 
{
	/*TO-DO: implement*/
	return "yumenikki";
}
std::string getServerURL() 
{
	return "ws://localhost:4040";
}
#endif

void MultiplayerStart() 
{
	Output::Debug("Multiplayer start");
	Connect(getServerURL(), getGameName(), Credentials());
	Output::Debug("Multiplayer started");
}

void ChangeMap(int map) 
{
	MultiplayerCharacter::ClearCharacters();
	sendPacket(MapPacket(map));
	setCurrentMapId(map);
}

void MultiplayerUpdate() 
{
	ConnectionUpdate();
	MultiplayerCharacter::UpdateCharacters();
}

void MultiplayerClose() 
{
	ConnectionClose();
}