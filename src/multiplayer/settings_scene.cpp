#include "settings_scene.h"
#include "connection.h"
#include "multiplayercharacter.h"
#include "settings.h"
#include "input.h"
#include "game_system.h"
#include "bitmap.h"
#include "output.h"
#include "font.h"
#include "player.h"
#include <algorithm>
#include <cmath>
#include <math.h>
#include <emscripten.h>
#include <emscripten/websocket.h>

namespace Game_Multiplayer {

	void LoadSetting(std::string name, int* ptr) {
		*ptr = EM_ASM_INT({
			let settingName = "MultiplayerSettings[" + UTF8ToString($0) + "]";
			return (window.localStorage.hasOwnProperty(settingName)) ? window.localStorage[settingName] : $1;
		}, name.c_str(), *ptr);
	}

	void SaveSetting(std::string name, int value) {
		EM_ASM({
			window.localStorage["MultiplayerSettings[" + UTF8ToString($0) + "]"] = $1;
		}, name.c_str(), value);
	}

	bool Reconnect(SettingsItem* item, Input::InputButton action) {
		emscripten_websocket_deinitialize();
		return true;
	}

	bool ToggleGlobalChatVisivility(SettingsItem* item, Input::InputButton action) {
		//chatBox->toggleVisibilityFlag(CV_GLOBAL);
		return true;
	}

	Window_Multiplayer::Window_Multiplayer(int ix, int iy, int iwidth, int iheight) : 
	Window_Selectable(ix, iy, iwidth, iheight) {
		this->column_max = 1;
		this->index = 0;

		this->UpdateHelpFn = [this](Window_Help& win, int index) {
			if (index >= 0 && index < static_cast<int>(this->items.size())) {
				win.SetText(items[index]->help);
			} else {
				win.SetText("");
			}
		};
		
		items.push_back(std::make_unique<SwitchOption>("SFX sync", "Lets you hear sound effects that other players produce", &Settings::sfxSync));
		items.push_back(std::make_unique<RangeOption>("SFX distance", "Maximum distance at which you can hear players", &Settings::sfxDistance, 4, 32, 1));
		items.push_back(std::make_unique<RangeOption>("SFX volume", "Volume of the sound effects that other players produce", &Settings::sfxVolume, 0, 100, 2));
		items.push_back(std::make_unique<SwitchOption>("Event movement synchronisation", "Lets you and other players see NPCs at same positions", &Settings::npcSync));
		items.push_back(std::make_unique<SwitchOption>("Event sprite synchronisation", "Lets you and other players see NPCs with the same sprites", &Settings::npcSpriteSync));
		items.push_back(std::make_unique<SwitchOption>("Event activity synchronisation", "Lets you and other players see same active NPCs", &Settings::npcActivitySync));
		items.push_back(std::make_unique<SwitchOption>("Own nametag", "Lets you see your own nametag", &Settings::drawOwnNametag));
		items.push_back(std::make_unique<SwitchOption>("Nametags", "Lets you see usernames of the players in game", &Settings::drawOtherNametags));
		items.push_back(std::make_unique<SwitchOption>("Nametags system sync", "Render nametags with system colors that player is using", &Settings::nametagSystemSync));
		items.push_back(std::make_unique<ActionOption>("Global chat", "Toggle global chat visibility", &ToggleGlobalChatVisivility));
		items.push_back(std::make_unique<ActionOption>("Reconnect", "Disconnect, clear players & connect again", &Reconnect));

		this->item_max = items.size();
		CreateContents();
		SetEndlessScrolling(false);
	}

	void Window_Multiplayer::Update() {
		Window_Selectable::Update();
	}

	void Window_Multiplayer::Refresh() {
		DrawItems();
	}

	void Window_Multiplayer::DrawItems() {
		for(int i = 0; i < items.size(); i++) {
			DrawItem(i);
		}
	}

	void Window_Multiplayer::DrawItem(int i) {
		contents->ClearRect(Rect(0, menu_item_height * i, contents->GetWidth() - 0, menu_item_height));
		contents->TextDraw(0, menu_item_height * i + menu_item_height / 8, items[i]->color, items[i]->name);
		contents->TextDraw(GetWidth() - 16, menu_item_height * i + menu_item_height / 8, items[i]->color_right, items[i]->text_right, Text::AlignRight);
	}

	void Window_Multiplayer::Action(Input::InputButton action) {
		items[index]->HandleAction(action);
	}

	Scene_MultiplayerSettings::Scene_MultiplayerSettings() {
		Scene::type = Scene::Multiplayer;
	}

	void Scene_MultiplayerSettings::Start() {
		settings_window.reset(new Window_Multiplayer(0, 32, SCREEN_TARGET_WIDTH, SCREEN_TARGET_HEIGHT - 32));
		help_window.reset(new Window_Help(0, 0, SCREEN_TARGET_WIDTH, 32));
		settings_window->SetHelpWindow(help_window.get());
		settings_window->Refresh();
	}
	void Scene_MultiplayerSettings::vUpdate() {
		settings_window->Update();
		help_window->Update();

		//exit settings event
		if (Input::IsTriggered(Input::CANCEL)) {
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Cancel));
			Scene::Pop();
		} else if (Input::IsTriggered(Input::DECISION)) {
			settings_window->Action(Input::DECISION);
			settings_window->Refresh();
		} else if (Input::IsTriggered(Input::LEFT)) {
			settings_window->Action(Input::LEFT);
			settings_window->Refresh();
		} else if (Input::IsTriggered(Input::RIGHT)) {
			settings_window->Action(Input::RIGHT);
			settings_window->Refresh();
		}
	}

	void ActionOption::HandleAction(Input::InputButton action) {
		if(this->onAction(this, action)) {
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
		} else {
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Buzzer));
		}
	}

	ActionOption::ActionOption(const std::string& name, const std::string& help, bool (*onAction) (SettingsItem* item, Input::InputButton action)) {
		this->name = name;
		this->help = help;
		this->onAction = onAction;
		this->color = Font::SystemColor::ColorDefault;
	}

	void SwitchOption::HandleAction(Input::InputButton action) {
		*(this->switch_ref) = !(*(this->switch_ref));
		this->text_right = *(this->switch_ref) ? "[ON]" : "[OFF]";
		this->color_right = *(this->switch_ref) ? Font::SystemColor::ColorDefault : Font::SystemColor::ColorDisabled;
		Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
		SaveSetting(name, *switch_ref);
	}

	SwitchOption::SwitchOption(const std::string& name, const std::string& help, bool* switch_ref) {
		LoadSetting(name, (int*)switch_ref);
		this->name = name;
		this->help = help;
		this->switch_ref = switch_ref;
		this->color = Font::SystemColor::ColorDefault;
		this->text_right = *(this->switch_ref) ? "[ON]" : "[OFF]";
		this->color_right = *(this->switch_ref) ? Font::SystemColor::ColorDefault : Font::SystemColor::ColorDisabled;
	}


	void RangeOption::HandleAction(Input::InputButton action) {
		if(action == Input::InputButton::LEFT) {
			*(this->range) -= this->step;
		} else if (action == Input::InputButton::RIGHT) {
			*(this->range) += this->step;
		}

		if(min > *range || *range > max) {
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Buzzer));
		} else {
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Cursor));
		}

		//for some reason i can't get std::clamp to work???
		if(*(this->range) > this->max) *(this->range) = this->max;
		if(*(this->range) < this->min) *(this->range) = this->min;

		this->text_right = std::to_string(*range);
		SaveSetting(name, *range);
	}
	RangeOption::RangeOption(const std::string& name, const std::string& help, int* range, int min, int max, int step) {
		LoadSetting(name, range);
		this->name = name;
		this->help = help;
		this->range = range;
		this->min = min;
		this->max = max;
		this->step = step;
		this->color = Font::SystemColor::ColorDefault;
		this->color_right = Font::SystemColor::ColorDefault;
		this->text_right = std::to_string(*range);
	}
}