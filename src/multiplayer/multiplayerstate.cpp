#include "multiplayerstate.h"
#include "settings.h"
#include "connection.h"
#include "packets/nametagpacket.h"
#include "packets/positionpacket.h"
#include "packets/movespeedpacket.h"
#include "packets/systempacket.h"
#include "init.h"
#include <emscripten.h>
#include <emscripten/websocket.h>
#include "sprite.h"
#include "game_player.h"
#include "main_data.h"
#include "drawable_mgr.h"
#include "scene_map.h"
#include "game_system.h"
#include "game_screen.h"
#include "nametags.h"
#include "output.h"

std::string clientUsername = "";
int currentMapId = -1;

std::string customSpriteSheet = "";
int customSpriteIndex = -1;
std::string realSpriteSheet = "";
int realSpriteIndex = -1;

std::unique_ptr<Sprite> own_nametag_sprite;

const std::string& getClientUsername()
{
	return clientUsername;
}
void setClientUsername(const std::string& username)
{
	if(username.length() > max_nametag_byte_length - 1) return;
	clientUsername = username;
	updateOwnNametagBitmap();
}

void setCurrentMapId(int map)
{
	currentMapId = map;
	#ifdef __EMSCRIPTEN__
	EM_ASM({
		window.parent.EasyRPGMultiplayer.setMapIdStatus(UTF8ToString($0));
	}, std::to_string(map).c_str());
	#endif
}

int getCurrentMapId()
{
	return currentMapId;
}

void sendAllSettings()
{
	if(currentMapId >= 0)
	{
		ChangeMap(currentMapId);
	}
	if(clientUsername.empty() == false)
	{
		ONametagPacket nametagPacket(clientUsername);
		sendMessage((unsigned char*)&nametagPacket, sizeof(nametagPacket));
	}
	OPositionPacket positionPacket(Main_Data::game_player->GetX(), Main_Data::game_player->GetY());
	sendMessage((unsigned char*)&positionPacket, sizeof(positionPacket));
	OMoveSpeedPacket speedPacket(Main_Data::game_player->GetMoveSpeed());
	sendMessage((unsigned char*)&speedPacket, sizeof(speedPacket));
	OSystemPacket systemPacket(std::string(Main_Data::game_system->GetSystemName()));
	sendMessage((unsigned char*)&systemPacket, sizeof(systemPacket));
}

bool getShouldDisplayOwnNametag()
{
	return Settings::drawOwnNametag;
}

void setShouldDisplayOwnNametag(bool value)
{
	Settings::drawOwnNametag = value;
}

void updateOwnNametag()
{
	if (!own_nametag_sprite) {
		auto scene_map = Scene::Find(Scene::SceneType::Map);
		if (scene_map) {
			auto old_list = &DrawableMgr::GetLocalList();
			DrawableMgr::SetLocalList(&scene_map->GetDrawableList());
			own_nametag_sprite = std::make_unique<Sprite>();
			DrawableMgr::SetLocalList(old_list);
			updateOwnNametagBitmap();
		}
	}

	if (own_nametag_sprite) {
		bool visible = getShouldDisplayOwnNametag() && Main_Data::game_player->IsVisible();
		own_nametag_sprite->SetVisible(visible);
		if (visible) {
			own_nametag_sprite->SetX(Main_Data::game_player->GetScreenX());
			own_nametag_sprite->SetY(Main_Data::game_player->GetScreenY() - 32 + 10);
			own_nametag_sprite->SetZ(Priority_EventsAbove + 1000);
			own_nametag_sprite->SetOpacity(Main_Data::game_player->GetOpacity());
			own_nametag_sprite->SetTone(Main_Data::game_screen->GetTone());
			own_nametag_sprite->SetFlashEffect(Main_Data::game_player->GetFlashColor());
		}
	}
}

void updateOwnNametagBitmap()
{
	// own_nametag_sprite is created lazily once a map scene exists. sendName
	// can arrive before that (e.g. right after the wasm runtime boots), so
	// guard against dereferencing a null sprite.
	if (!own_nametag_sprite) {
		return;
	}
	Nametags::UpdateNametagBitmap(*own_nametag_sprite, clientUsername, std::string(Main_Data::game_system->GetSystemName()), Main_Data::game_player.get());
}

bool getShouldDisplayNametags()
{
	return Settings::drawOtherNametags;
}
void setShouldDisplayNametags(bool value)
{
	Settings::drawOtherNametags = value;
}

bool hasCustomSprite()
{
	return !customSpriteSheet.empty() && customSpriteIndex >= 0;
}
void setCustomSprite(const std::string& sheet, int index)
{
	if(!hasCustomSprite())
	{
		storeRealSprite(Main_Data::game_player->GetSpriteName(), Main_Data::game_player->GetSpriteIndex());
	}
	customSpriteSheet = sheet;
	customSpriteIndex = index;
	Main_Data::game_player->SetSpriteGraphic(customSpriteSheet, customSpriteIndex);
}
void restoreRealSprite()
{
	customSpriteSheet = "";
	customSpriteIndex = -1;
	if(!realSpriteSheet.empty() && realSpriteIndex >= 0)
	{
		Main_Data::game_player->SetSpriteGraphic(realSpriteSheet, realSpriteIndex);
	}
}
void storeRealSprite(const std::string& sheet, int index)
{
	realSpriteSheet = sheet;
	realSpriteIndex = index;
	Output::Debug(sheet);
}

void getCustomSprite(std::string& sheet, int& index)
{
	sheet = customSpriteSheet;
	index = customSpriteIndex;
}

namespace Settings {

	bool Reconnect(Input::InputButton action) {
		emscripten_websocket_deinitialize();
		return true;
	}

	bool ToggleGlobalChatVisibility(Input::InputButton action) {
		//chatBox->toggleVisibilityFlag(CV_GLOBAL);
		return true;
	}

}