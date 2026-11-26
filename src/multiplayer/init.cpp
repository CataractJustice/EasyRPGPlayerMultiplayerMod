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
	updateOwnNametag();
	UpdateOwnTextBubbles();
}

void MultiplayerClose() 
{
	ConnectionClose();
}