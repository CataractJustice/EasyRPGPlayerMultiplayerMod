#include "init.h"
#include "connection.h"
#include "output.h"
#include "packets/mappacket.h"
#include "multiplayer/multiplayercharacter.h"
#include <emscripten.h>

#ifdef __EMSCRIPTEN__
std::string getGameName()
{
	const char* result = emscripten_run_script_string(
		"new URLSearchParams(window.location.search).get('game') || ''"
	);

	return result ? result : "";
}
#else
std::string getGameName() 
{
	/*TO-DO: implement*/
	return "yumenikki";
}
#endif

void MultiplayerStart() 
{
	Output::Debug("Multiplayer start");
	Connect("ws://localhost:4040", getGameName(), Credentials());
}

void ChangeMap(int map) 
{
	MultiplayerCharacter::ClearCharacters();
	sendPacket(MapPacket(map));
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