#pragma once
#include <string>
#include <vector>
#include "input.h"

// User configurable multiplayer settings plus their persistence and the data
// oriented description of the settings shown in the multiplayer settings menu.
// Runtime state and logic live in multiplayerstate.h.

namespace Settings
{
	extern bool sfxSync;
	extern int sfxVolume;
	extern int sfxDistance;
	extern bool npcSync;
	extern bool eventSync;
	extern bool npcSpriteSync;
	extern bool npcActivitySync;
	extern bool drawOwnNametag;
	extern bool drawOtherNametags;
	extern bool nametagSystemSync;
	extern bool chatBubbles;
	extern int chatBubbleFadeFrames;
	extern int chatBubbleFramesBeforeFade;
	extern int chatBubbleFloatSpeed;

	enum class SettingType { Switch, Range, Action };

	enum class ActionResult { Handled, Buzzer, Cursor };

	// Data oriented description of a single settings entry. A Switch targets a
	// bool, a Range targets an int within [min, max] and an Action invokes a
	// callback.
	struct SettingItem {
		std::string name;
		std::string help;
		SettingType type = SettingType::Switch;

		bool* boolTarget = nullptr;
		int* intTarget = nullptr;
		int min = 0;
		int max = 0;
		int step = 1;

		bool (*action)(Input::InputButton) = nullptr;
	};

	// Action callbacks referenced by the item list (implemented in the logic
	// layer, see multiplayerstate.cpp).
	bool Reconnect(Input::InputButton action);
	bool ToggleGlobalChatVisibility(Input::InputButton action);

	void LoadSetting(const std::string& name, int* ptr);
	void SaveSetting(const std::string& name, int value);

	// Loads every setting from persistent storage. Must be called once at
	// startup so settings are available before the settings menu is opened.
	void LoadAll();

	// The settings entries shown in the multiplayer settings menu.
	const std::vector<SettingItem>& GetItems();

	// Applies an action to an item, mutating and persisting its value.
	ActionResult HandleAction(const SettingItem& item, Input::InputButton action);
}