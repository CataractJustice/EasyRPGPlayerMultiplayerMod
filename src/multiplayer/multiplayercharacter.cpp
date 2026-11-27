#include "multiplayercharacter.h"
#include "main_data.h"
#include "output.h"
#include "drawable_mgr.h"
#include "scene_map.h"
#include "game_system.h"
#include "game_screen.h"
#include "cache.h"
#include "font.h"
#include "text.h"
#include "async_handler.h"
#include "nametags.h"
#include <cmath>
#include "settings.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#define MOVE_QUEUE_CAPACITY 8

void MultiplayerCharacter::MultiplayerMove(int x, int y) 
{
	this->moveQueue.push(std::pair<int, int>(x, y));
	if(moveQueue.size() > MOVE_QUEUE_CAPACITY)
		moveQueue.pop();
}

void MultiplayerCharacter::Update() 
{
	SetProcessed(false);
	Game_Character::Update();

	if (!this->sprite) {
		auto scene_map = Scene::Find(Scene::SceneType::Map);
		if (scene_map) {
			auto old_list = &DrawableMgr::GetLocalList();
			DrawableMgr::SetLocalList(&scene_map->GetDrawableList());
			this->sprite = std::make_unique<Sprite_Character>(this);
			DrawableMgr::SetLocalList(old_list);
		}
	}

	if (!this->nametag_sprite) {
		auto scene_map = Scene::Find(Scene::SceneType::Map);
		if (scene_map) {
			auto old_list = &DrawableMgr::GetLocalList();
			DrawableMgr::SetLocalList(&scene_map->GetDrawableList());
			this->nametag_sprite = std::make_unique<Sprite>();
			DrawableMgr::SetLocalList(old_list);
			Nametags::UpdateNametagBitmap(*this->nametag_sprite, this->nametag, this->systemname, this);
		}
	}

	if (this->sprite) {
		this->sprite->SetFlashEffect(Main_Data::game_screen->GetFlashColor());
		this->sprite->SetTone(Main_Data::game_screen->GetTone());
	}

	if (flashR || flashG || flashB || flashP || flashT) 
	{
		this->Flash(flashR, flashG, flashB, flashP, flashT);
	}

	if (this->IsVisible() && this->sprite) 
	{
		this->sprite->Update();
		this->sprite->SetFlashEffect(this->GetFlashColor());
	}

	if (this->nametag_sprite) {
		bool visible = getShouldDisplayNametags() && this->IsVisible() && !this->nametag.empty();
		this->nametag_sprite->SetVisible(visible);
		if (visible) {
			unsigned int stackHeight = 0;
			if(getShouldDisplayOwnNametag() && this->GetX() == Main_Data::game_player->GetX() && this->GetY() == Main_Data::game_player->GetY()) 
			{
				stackHeight++;
			}
			for(auto[id, character] : characters) 
			{
				if(id > this->id && character->GetX() == this->GetX() && character->GetY() == this->GetY()) 
				{
					stackHeight++;
				}
			}
			this->nametag_sprite->SetX(this->GetScreenX());
			this->nametag_sprite->SetY(this->GetScreenY() - 32 + 10 - (8) * stackHeight);
			this->nametag_sprite->SetZ(Priority_EventsAbove + 1000);
			this->nametag_sprite->SetOpacity(this->GetOpacity());
			this->nametag_sprite->SetTone(Main_Data::game_screen->GetTone());
			this->nametag_sprite->SetFlashEffect(this->GetFlashColor());
		}
	}

	if (this->IsStopping() && !moveQueue.empty()) 
	{
		std::pair<int, int> nextMove = moveQueue.front();
		moveQueue.pop();

		int dx = nextMove.first - GetX();
		int dy = nextMove.second - GetY();

		dx += 1;
		dy += 1;

		if (dx >= 0 && dy >= 0 && dx <= 2 && dy <= 2) {

			const int directions[3][3] = 
			{
				{Direction::UpLeft, 	Direction::Up, 		Direction::UpRight		},
				{Direction::Left, 		-1, 				Direction::Right		},
				{Direction::DownLeft,	Direction::Down, 	Direction::DownRight	}
			};

			int direction = directions[dy][dx];
			if (direction != -1)
				Move(direction);
		} 
		else 
		{
			SetX(nextMove.first);
			SetY(nextMove.second);
		}
	}

	// Update and remove message bubbles
	auto scene_map = Scene::Find(Scene::SceneType::Map);
	 for (auto it = text_bubbles.begin(); it != text_bubbles.end(); ) {
		  it->get()->Update();
		  if (it->get()->GetShouldBeDestroyed()) {
			auto old_list = &DrawableMgr::GetLocalList();
			DrawableMgr::SetLocalList(&scene_map->GetDrawableList());
				it = text_bubbles.erase(it);
			DrawableMgr::SetLocalList(old_list);
		  } else {
				++it;
		  }
	 }
}

void MultiplayerCharacter::UpdateNextMovementAction() {};

MultiplayerCharacter::MultiplayerCharacter(uint64_t id) : Game_PlayerBase(MPlayer)
{
	for(int i = 0; i < ACTOR_SLOTS; i++) 
	{
		actors[i] = nullptr;
	}
	//Uncomment to show user id as nametag for debugging 
	//this->nametag = std::to_string(id);
	this->systemname = (std::string)Main_Data::game_system->GetSystemName();
	this->id = id;
	this->SetSpriteGraphic(Main_Data::game_player->GetSpriteName(), Main_Data::game_player->GetSpriteIndex());
	this->SetX(Main_Data::game_player->GetX());
	this->SetY(Main_Data::game_player->GetY());
	this->SetLayer(Main_Data::game_player->GetLayer());
	this->SetMoveSpeed(Main_Data::game_player->GetMoveSpeed());
	this->SetMoveFrequency(Main_Data::game_player->GetMoveFrequency());
	this->SetThrough(true);

	auto old_list = &DrawableMgr::GetLocalList();
	auto scene_map = Scene::Find(Scene::SceneType::Map);
	if (scene_map) {
		DrawableMgr::SetLocalList(&scene_map->GetDrawableList());
	}
	sprite = std::make_unique<Sprite_Character>(this);
	nametag_sprite = std::make_unique<Sprite>();
	if (scene_map) {
		DrawableMgr::SetLocalList(old_list);
	}

	SetFlash(255, 255, 255, 0, 0);
	Nametags::UpdateNametagBitmap(*this->nametag_sprite, this->nametag, this->systemname, this);
	Output::Debug("New Multiplayer Character Created, id: {}.", id);
	
	#ifdef __EMSCRIPTEN__
	std::string userIdStr = std::to_string(this->id);
	EM_ASM({
		 (async () => {
			  try {
					const user = await window.parent.EasyRPGMultiplayer.getUsernameById(
						UTF8ToString($0)
					);

					console.log("Got user");
					console.log(user);
					if (!user || !user.username || !user.id)
						 return;

					console.log("Calling callback");
					window.easyrpgPlayer.setUserNametagById(user.id, user.username);
			  } catch (error) {
					console.error("Failed to load username:", error);
			  }
		 })();
	}, userIdStr.c_str());
	#endif
}

MultiplayerCharacter::~MultiplayerCharacter() 
{
	auto old_list = &DrawableMgr::GetLocalList();
	auto scene_map = Scene::Find(Scene::SceneType::Map);
	if (scene_map) {
		DrawableMgr::SetLocalList(&scene_map->GetDrawableList());
	}
	sprite.reset();
	nametag_sprite.reset();
	for (int i = 0; i < ACTOR_SLOTS; i++) 
	{
		delete actors[i];
		actors[i] = nullptr;
	}
	if (scene_map) {
		DrawableMgr::SetLocalList(old_list);
	}
}

void MultiplayerCharacter::OnSystemGraphicReady(FileRequestResult* result) 
{
	if (result && result->success) {
		Nametags::UpdateNametagBitmap(*this->nametag_sprite, this->nametag, this->systemname, this);
	}
}

void MultiplayerCharacter::SetSystem(const std::string& newsystem) 
{
	if (this->systemname != newsystem) {
		this->systemname = newsystem;
		if (!this->systemname.empty()) {
			FileRequestAsync* request = AsyncHandler::RequestFile("System", this->systemname);
			request->SetGraphicFile(true);
			system_request_id = request->Bind(&MultiplayerCharacter::OnSystemGraphicReady, this);
			request->Start();
		}
		Nametags::UpdateNametagBitmap(*this->nametag_sprite, this->nametag, this->systemname, this);
	}
}

void MultiplayerCharacter::SetNametag(const std::string& newnametag) 
{
	if (this->nametag != newnametag) {
		this->nametag = newnametag;
		Nametags::UpdateNametagBitmap(*this->nametag_sprite, this->nametag, this->systemname, this);
	}
}

void MultiplayerCharacter::MakeCharacter(uint64_t id) 
{
	characters[id] = std::make_shared<MultiplayerCharacter>(id);
}

void MultiplayerCharacter::UpdateCharacters() 
{
	for(auto map_pair : characters) 
	{
		map_pair.second->Update();
	}
}

void MultiplayerCharacter::RemovePlayer(uint64_t id) 
{
	auto characterIterator = characters.find(id);
	if(characterIterator == characters.cend()) 
	{
		Output::Warning("Trying to remove non existing multiplayer character with id: {}", id);
		return;
	}

	auto old_list = &DrawableMgr::GetLocalList();
	auto scene_map = Scene::Find(Scene::SceneType::Map);
	if (scene_map) {
		DrawableMgr::SetLocalList(&scene_map->GetDrawableList());
	}
	characters.erase(characterIterator);
	if (scene_map) {
		DrawableMgr::SetLocalList(old_list);
	}
	Output::Debug("Removed player, id: {}.", id);
}

std::shared_ptr<MultiplayerCharacter> MultiplayerCharacter::GetPlayer(uint64_t id) 
{
	auto characterIterator = characters.find(id);
	if(characterIterator == characters.cend()) 
		return std::shared_ptr<MultiplayerCharacter>(nullptr);
	
	return (*characterIterator).second;
}

void MultiplayerCharacter::ClearCharacters() 
{
	auto old_list = &DrawableMgr::GetLocalList();
	auto scene_map = Scene::Find(Scene::SceneType::Map);
	if (scene_map) {
		DrawableMgr::SetLocalList(&scene_map->GetDrawableList());
	}
	characters.clear();
	if (scene_map) {
		DrawableMgr::SetLocalList(old_list);
	}
}

float MultiplayerCharacter::GetDistanceToPlayer() 
{
	auto& player = Main_Data::game_player;
	auto& map = Game_Map::GetMap();

	float dx1 = player->GetX() - this->GetX();
	float dy1 = player->GetY() - this->GetY();
	
	float dx2 = player->GetX() - (this->GetX() - map.width);
	float dy2 = player->GetY() - (this->GetY() - map.height);

	float dx = std::min(dx1, dx2);
	float dy = std::min(dy1, dy2);

	return std::sqrt(dx * dx + dy * dy);
}

void MultiplayerCharacter::PlaySound(lcf::rpg::Sound sound) 
{
	float l = GetDistanceToPlayer();

	sound.volume = sound.volume * ((float)Settings::sfxVolume) * 0.01f * std::max(1.0f - l / ((float)Settings::sfxDistance), 0.0f);

	Main_Data::game_system->SePlay(sound);
}

std::map<uint64_t, std::shared_ptr<MultiplayerCharacter>> MultiplayerCharacter::characters = std::map<uint64_t, std::shared_ptr<MultiplayerCharacter>>();

void FullresDrawPass(BitmapRef&, int) 
{
}

void MultiplayerCharacter::UpdateActor(IActorInfoPacket* packet, int actorIndex) 
{
	if(!this->actors[actorIndex]) this->actors[actorIndex] = new Game_Actor(0);
	Game_Actor& actor = *this->actors[actorIndex];

	actor.SetBaseMaxHp(packet->baseMaxHp);
	actor.SetBaseMaxSp(packet->baseMaxSp);
	actor.SetHp(packet->currentHp);
	actor.SetSp(packet->currentSp);
	actor.SetName(std::string(packet->name));
	actor.SetTitle(std::string(packet->title));
	actor.SetSprite(std::string(packet->spriteName), (int)packet->spriteId, false);
	actor.SetFace(std::string(packet->faceName), (int)packet->faceId);
	actor.SetLevel(packet->level);
}

std::vector<Game_Actor*> MultiplayerCharacter::GetAllParties() 
{
	std::vector<Game_Actor*> mpPartyActors;
	for(auto& character : characters) 
	{
		for(int i = 0; i < ACTOR_SLOTS; i++) 
		{
			if(character.second->actors[i]) 
			{
				mpPartyActors.push_back(character.second->actors[i]);
			}
		}
	}
	return mpPartyActors;
}

void MultiplayerCharacter::CreateMessageBubble(const std::string text) 
{
	if(text_bubbles.empty() == false) 
	{
		text_bubbles.back()->FadeNow();
	}
	auto old_list = &DrawableMgr::GetLocalList();
	auto scene_map = Scene::Find(Scene::SceneType::Map);
	if (scene_map) {
		DrawableMgr::SetLocalList(&scene_map->GetDrawableList());
	}

	// Create a new TextBubble and add it to the list
	text_bubbles.push_back(std::make_unique<TextBubble>(text, this));

	if (scene_map) {
		DrawableMgr::SetLocalList(old_list);
	}
}

void MultiplayerCharacter::OnMapDispose() 
{
	ClearCharacters();
}

int MultiplayerCharacter::GetPlayerCount() 
{
	return MultiplayerCharacter::characters.size() + 1;
}