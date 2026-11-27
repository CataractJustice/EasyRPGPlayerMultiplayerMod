#include "settings.h"
#include <emscripten.h>

namespace Settings
{
	bool sfxSync = true;
	int sfxVolume = 100;
	int sfxDistance = 10;
	bool npcSync = false;
	bool eventSync = false;
	bool npcSpriteSync = true;
	bool npcActivitySync = false;
	bool drawOwnNametag = true;
	bool drawOtherNametags = true;
	bool nametagSystemSync = true;
	bool chatBubbles = true;
	int chatBubbleFadeFrames = 60;
	int chatBubbleFramesBeforeFade = 60;
	int chatBubbleFloatSpeed = 4;

	void LoadSetting(const std::string& name, int* ptr) {
		*ptr = EM_ASM_INT({
			let settingName = "MultiplayerSettings[" + UTF8ToString($0) + "]";
			return (window.localStorage.hasOwnProperty(settingName)) ? window.localStorage[settingName] : $1;
		}, name.c_str(), *ptr);
	}

	void SaveSetting(const std::string& name, int value) {
		EM_ASM({
			window.localStorage["MultiplayerSettings[" + UTF8ToString($0) + "]"] = $1;
		}, name.c_str(), value);
	}

	void LoadAll() {
		for (const SettingItem& item : GetItems()) {
			if (item.type == SettingType::Switch && item.boolTarget) {
				LoadSetting(item.name, (int*)item.boolTarget);
			} else if (item.type == SettingType::Range && item.intTarget) {
				LoadSetting(item.name, item.intTarget);
			}
		}
	}

	const std::vector<SettingItem>& GetItems() {
		static const std::vector<SettingItem> items = {
			{ "SFX sync", "Lets you hear sound effects that other players produce", SettingType::Switch, &sfxSync },
			{ "SFX distance", "Maximum distance at which you can hear players", SettingType::Range, nullptr, &sfxDistance, 4, 32, 1 },
			{ "SFX volume", "Volume of the sound effects that other players produce", SettingType::Range, nullptr, &sfxVolume, 0, 100, 2 },
			{ "Event movement synchronisation", "Lets you and other players see NPCs at same positions", SettingType::Switch, &npcSync },
			{ "Event sprite synchronisation", "Lets you and other players see NPCs with the same sprites", SettingType::Switch, &npcSpriteSync },
			{ "Event activity synchronisation", "Lets you and other players see same active NPCs", SettingType::Switch, &npcActivitySync },
			{ "Own nametag", "Lets you see your own nametag", SettingType::Switch, &drawOwnNametag },
			{ "Nametags", "Lets you see usernames of the players in game", SettingType::Switch, &drawOtherNametags },
			{ "Nametags system sync", "Render nametags with system colors that player is using", SettingType::Switch, &nametagSystemSync },
			{ "Global chat", "Toggle global chat visibility", SettingType::Action, nullptr, nullptr, 0, 0, 1, &ToggleGlobalChatVisibility },
			{ "Reconnect", "Disconnect, clear players & connect again", SettingType::Action, nullptr, nullptr, 0, 0, 1, &Reconnect },
			{ "Chat bubbles", "Show sent messages above players who sent them", SettingType::Switch, &chatBubbles },
			{ "Chat bubble show frames", "How long chat bubble should stay before fading away", SettingType::Range, nullptr, &chatBubbleFramesBeforeFade, 20, 240, 20 },
			{ "Chat bubble fade frames", "How long does it take chat bubble to fade", SettingType::Range, nullptr, &chatBubbleFadeFrames, 0, 240, 20 },
			{ "Chat bubble float speed", "How fast should chat bubble float up", SettingType::Range, nullptr, &chatBubbleFloatSpeed, 0, 16, 1 },
		};
		return items;
	}

	ActionResult HandleAction(const SettingItem& item, Input::InputButton action) {
		switch (item.type) {
		case SettingType::Switch:
			if (item.boolTarget) {
				*item.boolTarget = !*item.boolTarget;
				SaveSetting(item.name, (int)*item.boolTarget);
			}
			return ActionResult::Handled;
		case SettingType::Range: {
			if (!item.intTarget) {
				return ActionResult::Handled;
			}
			if (action == Input::InputButton::LEFT) {
				*item.intTarget -= item.step;
			} else if (action == Input::InputButton::RIGHT) {
				*item.intTarget += item.step;
			}

			ActionResult result = (*item.intTarget < item.min || *item.intTarget > item.max) ? ActionResult::Buzzer : ActionResult::Cursor;

			if (*item.intTarget > item.max) *item.intTarget = item.max;
			if (*item.intTarget < item.min) *item.intTarget = item.min;

			SaveSetting(item.name, *item.intTarget);
			return result;
		}
		case SettingType::Action:
			return (item.action && item.action(action)) ? ActionResult::Handled : ActionResult::Buzzer;
		}
		return ActionResult::Handled;
	}
}