#include "init.h"
#include "connection.h"
#include "output.h"
#include "packets/mappacket.h"
#include "multiplayer/multiplayercharacter.h"
#include <emscripten.h>
#include "settings.h"
#include "textbubble.h"
#include "main_data.h"
#include "drawable_mgr.h"
#include "scene.h"

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

Credentials getCredentials()
{
    Credentials creds;
    const char* authToken = emscripten_run_script_string(
        "window.parent.EasyRPGMultiplayer.getAuthToken ? window.parent.EasyRPGMultiplayer.getAuthToken() : ''"
    );
    if (authToken && strlen(authToken) > 0) {
        creds.authToken = authToken;
    }
    
    const char* accountIdStr = emscripten_run_script_string(
        "window.parent.EasyRPGMultiplayer.getAccountId ? window.parent.EasyRPGMultiplayer.getAccountId() : ''"
    );
    if (accountIdStr && strlen(accountIdStr) > 0) {
        creds.accountId = strtoull(accountIdStr, nullptr, 10);
    }
    
    const char* accountType = emscripten_run_script_string(
        "window.parent.EasyRPGMultiplayer.getAccountType ? window.parent.EasyRPGMultiplayer.getAccountType() : ''"
    );
    if (accountType && strlen(accountType) > 0) {
        creds.accountType = accountType;
    }
    
    const char* username = emscripten_run_script_string(
        "window.parent.EasyRPGMultiplayer.getUsername ? window.parent.EasyRPGMultiplayer.getUsername() : ''"
    );
    if (username && strlen(username) > 0) {
        creds.username = username;
    }
    
    const char* guestUuid = emscripten_run_script_string(
        "window.parent.EasyRPGMultiplayer.getGuestUuid ? window.parent.EasyRPGMultiplayer.getGuestUuid() : ''"
    );
    if (guestUuid && strlen(guestUuid) > 0) {
        creds.guestUuid = guestUuid;
    }
    
    return creds;
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

Credentials getCredentials()
{
    return Credentials();
}
#endif

std::deque<std::unique_ptr<TextBubble>> ownTextBubbles;

void CreateOwnTextBubble(const std::string& text) 
{
	auto old_list = &DrawableMgr::GetLocalList();
	auto scene_map = Scene::Find(Scene::SceneType::Map);
	if (scene_map) {
		DrawableMgr::SetLocalList(&scene_map->GetDrawableList());
	}
	if(!ownTextBubbles.empty()) 
	{
		ownTextBubbles.back()->FadeNow();
	}
	ownTextBubbles.emplace_back(std::make_unique<TextBubble>(text, Main_Data::game_player.get()));
	if (scene_map) {
		DrawableMgr::SetLocalList(old_list);
	}
}

void UpdateOwnTextBubbles() 
{
	auto old_list = &DrawableMgr::GetLocalList();
	auto scene_map = Scene::Find(Scene::SceneType::Map);
	if (scene_map) {
		DrawableMgr::SetLocalList(&scene_map->GetDrawableList());
	}
	for(auto& textBubble : ownTextBubbles) 
	{
		textBubble->Update();
	}
	while (!ownTextBubbles.empty())
	{
		TextBubble& textBubble = *ownTextBubbles.front().get();
		if(textBubble.GetShouldBeDestroyed()) 
		{
			ownTextBubbles.pop_front();
		}
		else 
		{
			break;
		}
	}
	if (scene_map) {
		DrawableMgr::SetLocalList(old_list);
	}
}

void MultiplayerStart() 
{
	Output::Debug("Multiplayer start");
	Credentials creds = getCredentials();
	Connect(getServerURL(), getGameName(), creds);
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
	updateOwnNametag();
	UpdateOwnTextBubbles();
}

void MultiplayerClose() 
{
	ConnectionClose();
}