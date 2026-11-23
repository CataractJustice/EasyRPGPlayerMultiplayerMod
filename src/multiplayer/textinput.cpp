#include "textinput.h"
#include "drawable_mgr.h"
#include <SDL3/SDL_events.h>
#include "font.h"
#include "text.h"
#include "cache.h"
#include "output.h"

TextInput::TextInput(int x, int y, int w, int h, std::shared_ptr<Scene> scene) 
{
	this->scroll = 0;
	auto old_list = &DrawableMgr::GetLocalList();
	DrawableMgr::SetLocalList(&scene->GetDrawableList());
	this->window = std::make_unique<Window_Base>(x, y, w, h);
	DrawableMgr::SetLocalList(old_list);
	this->input = "";
	this->cursor = 0;
	this->window->SetContents(Bitmap::Create(x, y, w, h));
}

void TextInput::ProcessTextInput(SDL_Event* evnt) 
{
	if(input.length() >= 511) return;
	this->input = input.insert(cursor, evnt->text.text);
	cursor++;

	RefreshInput();
}
void TextInput::ProcessKeyDown(SDL_Event* evnt) 
{
	switch (evnt->key.key)
	{
	case SDLK_LEFT:
		cursor--;
		break;
	case SDLK_RIGHT:
		cursor++;
		break;
	case SDLK_BACKSPACE:
	if(cursor) 
	{
		this->input.erase(cursor - 1, 1);
		cursor--;
	}
	break;
	default:
		break;
	}

	RefreshInput();
}

void TextInput::RefreshInput() 
{
	cursor = cursor < 0 ? 0 : cursor;
	cursor = cursor > input.length() ? input.length() : cursor;

	std::string inputBeforeCaret = input;
	inputBeforeCaret.erase(cursor);
	auto rect = Text::GetSize(*Font::Default(), inputBeforeCaret);

	const int scrollPadding = 16;

	if(rect.width > scroll + window.get()->GetWidth() - scrollPadding*2) 
	{
		scroll += rect.width - scroll - window.get()->GetWidth() + scrollPadding*2;
	}
	if(rect.width < scroll + scrollPadding) 
	{
		scroll += rect.width - scroll - scrollPadding;
	}

	window->GetContents()->Clear();
	window->GetContents()->TextDraw( -scroll, 0, Font::SystemColor::ColorDefault, input);
	window->GetContents()->TextDraw( -scroll + rect.width - Text::GetSize(*Font::Default(), "|").width / 2, 0, Font::SystemColor::ColorCritical, "|");
}