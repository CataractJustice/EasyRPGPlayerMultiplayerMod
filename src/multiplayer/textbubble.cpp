// text_bubble.cpp
#include "textbubble.h"
#include "game_character.h"
#include "sprite.h"
#include "game_player.h"
#include "main_data.h"
#include "drawable_mgr.h"
#include "scene_map.h"
#include "game_system.h"
#include "game_screen.h"
#include "cache.h"
#include "multiplayercharacter.h"
#include "settings.h"

TextBubble::TextBubble(const std::string& text, const Game_Character* character)
	: sprite(), text(text), frame(0), character(character) {
	FontRef font = Font::Tiny();
	if (!font) {
		return;
	}

	Rect text_size = Text::GetSize(*font, text);
	if (text_size.width <= 0 || text_size.height <= 0) {
		return;
	}

	constexpr int padding = 0;
	int w = text_size.width + padding * 2;
	int h = text_size.height + padding * 2;

	BitmapRef bmp = Bitmap::Create(w, h, Color(0, 0, 0, 0));
	if (!bmp) {
		return;
	}
	
	const MultiplayerCharacter* MPChar = dynamic_cast<const MultiplayerCharacter*>(character);

	if(MPChar) 
	{
		BitmapRef system = Cache::System(MPChar->GetSystem());
		if (!system) {
			system = Cache::SystemOrBlack();
		}
		Text::Draw(*bmp, padding, padding, *font, *system, 0, text);
	}
	else 
	{
		Text::Draw(*bmp, padding, padding, *font, *Cache::SystemOrBlack(), 0, text);
	}

	sprite.SetBitmap(bmp);
	sprite.SetOx(w / 2);
	sprite.SetOy(-h);
	sprite.SetX(character->GetScreenX());
	sprite.SetY(character->GetScreenY() - 58);
	originalY = sprite.GetY();
	sprite.SetZ(Priority_EventsAbove + 1000);
	sprite.SetTone(Main_Data::game_screen->GetTone());
	sprite.SetFlashEffect(character->GetFlashColor());
}

TextBubble::~TextBubble() {}

void TextBubble::Update() 
{
	frame++;
	if(frame == Settings::chatBubbleFramesBeforeFade) 
	{
		originalY = sprite.GetY();
	}
	if(frame > Settings::chatBubbleFramesBeforeFade) 
	{
		int framesFading = frame - Settings::chatBubbleFramesBeforeFade;
		sprite.SetY(originalY - framesFading * Settings::chatBubbleFloatSpeed / 8);
		sprite.SetOpacity(255 - framesFading * (255 / Settings::chatBubbleFadeFrames));
	}
	else 
	{
		sprite.SetY(character->GetScreenY() - 58);
		sprite.SetX(character->GetScreenX());
	}
}

bool TextBubble::GetShouldBeDestroyed() 
{
	return frame > Settings::chatBubbleFadeFrames + Settings::chatBubbleFramesBeforeFade;
}

void TextBubble::FadeNow() 
{
	originalY = sprite.GetY();
	this->frame = frame < Settings::chatBubbleFramesBeforeFade ? Settings::chatBubbleFramesBeforeFade : frame;
	this->frame += 4 * 8 / Settings::chatBubbleFloatSpeed;
}