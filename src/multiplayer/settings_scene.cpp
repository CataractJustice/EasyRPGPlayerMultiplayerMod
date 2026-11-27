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

	namespace {

		std::string GetRightText(const Settings::SettingItem& item) {
			switch (item.type) {
			case Settings::SettingType::Switch:
				return (item.boolTarget && *item.boolTarget) ? "[ON]" : "[OFF]";
			case Settings::SettingType::Range:
				return item.intTarget ? std::to_string(*item.intTarget) : "";
			default:
				return "";
			}
		}

		Font::SystemColor GetRightColor(const Settings::SettingItem& item) {
			if (item.type == Settings::SettingType::Switch) {
				return (item.boolTarget && *item.boolTarget) ? Font::SystemColor::ColorDefault : Font::SystemColor::ColorDisabled;
			}
			return Font::SystemColor::ColorDefault;
		}

	}

	Window_Multiplayer::Window_Multiplayer(int ix, int iy, int iwidth, int iheight) :
	Window_Selectable(ix, iy, iwidth, iheight) {
		this->column_max = 1;
		this->index = 0;

		this->UpdateHelpFn = [this](Window_Help& win, int index) {
			const auto& items = Settings::GetItems();
			if (index >= 0 && index < static_cast<int>(items.size())) {
				win.SetText(items[index].help);
			} else {
				win.SetText("");
			}
		};

		this->item_max = static_cast<int>(Settings::GetItems().size());
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
		const auto& items = Settings::GetItems();
		for(int i = 0; i < static_cast<int>(items.size()); i++) {
			DrawItem(i);
		}
	}

	void Window_Multiplayer::DrawItem(int i) {
		const auto& items = Settings::GetItems();
		const Settings::SettingItem& item = items[i];
		contents->ClearRect(Rect(0, menu_item_height * i, contents->GetWidth() - 0, menu_item_height));
		contents->TextDraw(0, menu_item_height * i + menu_item_height / 8, Font::SystemColor::ColorDefault, item.name);
		contents->TextDraw(GetWidth() - 16, menu_item_height * i + menu_item_height / 8, GetRightColor(item), GetRightText(item), Text::AlignRight);
	}

	void Window_Multiplayer::Action(Input::InputButton action) {
		const auto& items = Settings::GetItems();
		Settings::ActionResult result = Settings::HandleAction(items[index], action);
		if (result == Settings::ActionResult::Buzzer) {
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Buzzer));
		} else if (result == Settings::ActionResult::Cursor) {
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Cursor));
		} else {
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
		}
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
}