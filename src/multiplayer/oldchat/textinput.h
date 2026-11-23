#pragma once
#include "bitmap.h"
#include "window_base.h"
#include "options.h"
#include "scene.h"
#include <memory>

extern "C" 
{
	union SDL_Event;
}

class TextInput 
{
	private:
	int cursor;
	std::string input;
	std::unique_ptr<Window_Base> window;
	int scroll;
	static TextInput* focused;
	public:
	TextInput(int x, int y, int w, int h, std::shared_ptr<Scene> scene);
	void ProcessTextInput(SDL_Event*);
	void ProcessKeyDown(SDL_Event*);
	void RefreshInput();
	inline std::string& GetInput() { return input; };
	inline void Focus() { focused = this; }
	static inline void Unfocus() { focused = nullptr; }
	static inline TextInput* CurrentFocusedInput() { return focused; }
	std::unique_ptr<Window_Base>& GetWindow() { return window; }
};