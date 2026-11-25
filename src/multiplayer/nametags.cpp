#include "nametags.h"
#include "game_character.h"
#include "sprite.h"
#include "game_player.h"
#include "main_data.h"
#include "drawable_mgr.h"
#include "scene_map.h"
#include "game_system.h"
#include "game_screen.h"
#include "cache.h"
namespace Nametags 
{
	void UpdateNametagBitmap(Sprite& nametag_sprite, const std::string& nametag, const std::string& systemname, const Game_Character* character)
	{
		if (nametag.empty()) {
			nametag_sprite.SetBitmap(nullptr);
			return;
		}

		FontRef font = Font::Tiny();
		if (!font) {
			return;
		}

		Rect text_size = Text::GetSize(*font, nametag);
		if (text_size.width <= 0 || text_size.height <= 0) {
			return;
		}

		BitmapRef system = Cache::System(systemname);
		if (!system) {
			system = Cache::SystemOrBlack();
		}

		constexpr int padding = 0;
		int w = text_size.width + padding * 2;
		int h = text_size.height + padding * 2;

		BitmapRef bmp = Bitmap::Create(w, h, Color(0, 0, 0, 0));
		if (!bmp) {
			return;
		}

		Text::Draw(*bmp, padding, padding, *font, *system, 0, nametag);

		nametag_sprite.SetBitmap(bmp);
		nametag_sprite.SetOx(w / 2);
		nametag_sprite.SetOy(h);
	}
}