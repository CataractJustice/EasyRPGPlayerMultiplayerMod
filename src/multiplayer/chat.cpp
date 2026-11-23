#include "chat.h"
#include "font.h"
#include "text.h"
#include "cache.h"
#include "output.h"
#include "drawable_mgr.h"
#include <string>
#include <vector>
#include <SDL3/SDL_events.h>
#include "connection.h"
#include "packets/chatmsgpacket.h"

#define line_height 12
#define chat_log_height 1280
#define CHAR_WIDTH 32

Chat::Chat(std::shared_ptr<Scene> scene, int zoom) : textInput(SCREEN_TARGET_WIDTH * zoom + 16, SCREEN_TARGET_HEIGHT * zoom - line_height * 2 - 8, CHAT_WIDTH * zoom - 32,  + line_height * 2 + 8, scene)
{
	this->zoom = zoom;
	auto old_list = &DrawableMgr::GetLocalList();
	DrawableMgr::SetLocalList(&scene->GetDrawableList());
	this->window = std::make_unique<Window_Base>(SCREEN_TARGET_WIDTH * zoom, -4, CHAT_WIDTH * zoom + 4, SCREEN_TARGET_HEIGHT * zoom + 8);
	window->SetContents(Bitmap::Create(CHAT_WIDTH * zoom, SCREEN_TARGET_HEIGHT * zoom));
	window->GetContents()->Clear();
	window->SetZ(10000);
	window->GetContents()->TextDraw(0, 0, Font::ColorDefault, "Chat");
	DrawableMgr::SetLocalList(old_list);
	this->messageLogBitmap = Bitmap::Create(CHAT_WIDTH * zoom, chat_log_height * zoom);
	this->needsRefresh = true;
	this->focus = false;
	this->logHeight = 0;
	this->scroll = 0;
}

void Chat::SetFocus(bool focus) 
{
	this->focus = focus;
}

bool Chat::GetFocus() 
{
	return this->focus;
}

void Chat::Update() 
{
	if(focus) 
	{
		
	}
}

void Chat::PushMessage(const Message& message)
{
	messageLog.push_back(message);
	if(logHeight > messageLogBitmap->GetHeight()) 
	{
		messageLogBitmap->Clear();
		messageLogBitmap->Blit(0, messageLogBitmap->GetHeight() - logHeight, *messageLogBitmap, messageLogBitmap->GetRect(), Opacity::Opaque());
		logHeight = messageLogBitmap->GetHeight();
	}
	if((logHeight - scroll) > (SCREEN_TARGET_HEIGHT * zoom - 64)) 
	{
		scroll = logHeight - (SCREEN_TARGET_HEIGHT * zoom - 64);
	}
	messageLogBitmap->Blit(0, logHeight, *(message.messageBitmap), message.messageBitmap->GetRect(), Opacity::Opaque());
	logHeight += message.messageBitmap->GetHeight() + 8;
	needsRefresh = true;
}

void Chat::PushMessage(IChatMsgPacket* message) 
{
	PushMessage(Message(message->ownerId, (Message::MessageType)message->scope, "Who", message->message, 100, zoom));
}

void Chat::Draw(Bitmap& bitmap) 
{
	if(needsRefresh)
	{
		window->GetContents()->Clear();
		window->GetContents()->Blit(0, 18, *messageLogBitmap, Rect(0, scroll, messageLogBitmap->GetRect().width, window->GetContents()->GetRect().height), Opacity::Opaque());
		needsRefresh = false;
	}
	window->Draw(bitmap);
	textInput.GetWindow()->Draw(bitmap);
}

std::string& Chat::GetInputString() 
{
	return textInput.GetInput();
}

void Chat::FireMessage() 
{
	if(GetInputString().length() == 0) return;
	sendPacket(OChatMsgPacket(GetInputString(), DataPacketFlag::broadcastForMap));
	PushMessage(Message(-1, Message::map, "Me", GetInputString(), 100, zoom));
	GetInputString().erase();
}

Message::Message(int ownerId, Message::MessageType type, const std::string& nametag, const std::string& text, int maxLines, int zoom) : 
ownerId(ownerId), 
type(type), 
nametag(nametag), 
text(text)
{
	std::string prefix;

	switch (type)
	{
	case Message::map:
		prefix = "[M]";
		break;
	case Message::game:
		prefix = "[G]";
		break;
	case Message::server:
		prefix = "[S]";
		break;
	case Message::party:
		prefix = "[P]";
		break;
	case Message::direct:
		prefix = "[D]";
		break;
	default:
		break;
	}
	std::string header = prefix + nametag;
	Rect headerRect = Text::GetSize(*Font::Default(), header);

	int w = CHAT_WIDTH * zoom;
	int maxLineChars = w / CHAR_WIDTH;

	std::string wrappedText;

	std::vector<std::string> lines;
  
	for(int i = 0; i < text.length(); i++) 
	{
		//skip new lines
		if(text[i] == '\n') 
		{
			wrappedText += ' ';
			continue;
		}
		
		if(text[i] == ' ') 
		{
			//check if previous character was space and skip current space if it was
			if(i > 0) 
			{
				if(text[i - 1] == ' ') 
				{
					continue;
				}
			}
			//getting length of next word
			bool nextWordFound = false;
			int nextWordLength = 0;
			for(int j = i + 1; j < text.length(); j++) 
			{
				if(!nextWordFound) 
				{
				//if no word was found yet check if it's the begining of a word
					if(text[j] != ' ' && text[j] != '\n') 
					{
						nextWordFound = true;
					}
				}
				//if a word was found then increment word length untill space or a new line
				if(nextWordFound) 
				{
					if(text[j] == ' ' || text[j] == '\n') break;
					nextWordLength++;
				}
			}
			//if current line length plus next word is greater than max characters per line, add a new line before next, word reset line length and continue
			if((nextWordLength + wrappedText.length()) > maxLineChars) 
			{
				lines.push_back(wrappedText);
				wrappedText = "";
				continue;
			}
		}
		//force break if word is too long for a single line
		if(wrappedText.length() > maxLineChars) 
		{
			lines.push_back(wrappedText);
			wrappedText = "";
			continue;
		}

		wrappedText += text[i];
	}
	lines.push_back(wrappedText);
	
	int h = headerRect.height + lines.size() * line_height;
	messageBitmap = Bitmap::Create(w, h);

	messageBitmap->TextDraw(0, 0, Font::SystemColor::ColorDisabled, header);

	for(int i = 0; i < lines.size(); i++) 
	{
		Text::Draw(*messageBitmap, 0, headerRect.height + i * line_height, *Font::Default(), *Cache::SystemOrBlack(), 0, lines[i]);
	}
}

Chat* Chat::chatInstance = nullptr;

bool Chat::HasInstance() 
{
	return Chat::chatInstance != nullptr;
}

void Chat::MakeInstance(std::shared_ptr<Scene> scene, int zoom) 
{
	Chat::chatInstance = new Chat(scene, zoom);
}

void Chat::ProcessTextInput(SDL_Event* evnt) 
{
	textInput.ProcessTextInput(evnt);
}

void Chat::ProcessKeyDown(SDL_Event* evnt)
{
	switch (evnt->key.key)
	{
		case SDLK_UP:
			scroll -= 16;
			needsRefresh = true;
			break;

		case SDLK_DOWN:
			scroll += 16;
			needsRefresh = true;
			break;

		case SDLK_RETURN:
			this->FireMessage();
			break;

		default:
			break;
	}

	textInput.ProcessKeyDown(evnt);
}


int Chat::Zoom() {return zoom;}

Chat& Chat::Instance() { return *chatInstance; }