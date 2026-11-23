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
#include <cmath>

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
			UpdateNametagBitmap();
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
		bool visible = this->IsVisible() && !this->nametag.empty();
		this->nametag_sprite->SetVisible(visible);
		if (visible) {
			unsigned int stackHeight = 0;
			for(auto[id, character] : characters) 
			{
				if(id > this->id && character->GetX() == this->GetX() && character->GetY() == this->GetY()) 
				{
					stackHeight++;
				}
			}
			this->nametag_sprite->SetX(this->GetScreenX());
			int chara_height = (this->sprite && this->sprite->GetHeight() > 0) ? this->sprite->GetHeight() : 32;
			this->nametag_sprite->SetY(this->GetScreenY() - chara_height + 10 - (8) * stackHeight);
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
}

void MultiplayerCharacter::UpdateNextMovementAction() {};

MultiplayerCharacter::MultiplayerCharacter(unsigned int id) : Game_PlayerBase(MPlayer)
{
	for(int i = 0; i < ACTOR_SLOTS; i++) 
	{
		actors[i] = nullptr;
	}
	this->nametag = std::to_string(id);
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
	UpdateNametagBitmap();
	Output::Debug("New Multiplayer Character Created, id: {}.", id);
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

void MultiplayerCharacter::UpdateNametagBitmap() 
{
	if (this->nametag.empty()) {
		if (nametag_sprite) {
			nametag_sprite->SetBitmap(nullptr);
		}
		return;
	}

	FontRef font = Font::Tiny();
	if (!font) {
		return;
	}

	Rect text_size = Text::GetSize(*font, this->nametag);
	if (text_size.width <= 0 || text_size.height <= 0) {
		return;
	}

	BitmapRef system = Cache::System(this->systemname);
	if (!system) {
		system = Cache::SystemOrBlack();
	}

	int padding = 0;
	int w = text_size.width + padding * 2;
	int h = text_size.height + padding * 2;
	BitmapRef bmp = Bitmap::Create(w, h, Color(0, 0, 0, 0));
	if (!bmp) {
		return;
	}

	Text::Draw(*bmp, padding, padding, *font, *system, 0, this->nametag);

	if (!nametag_sprite) {
		auto scene_map = Scene::Find(Scene::SceneType::Map);
		if (scene_map) {
			auto old_list = &DrawableMgr::GetLocalList();
			DrawableMgr::SetLocalList(&scene_map->GetDrawableList());
			nametag_sprite = std::make_unique<Sprite>();
			DrawableMgr::SetLocalList(old_list);
		}
	}

	if (nametag_sprite) {
		nametag_sprite->SetBitmap(bmp);
		nametag_sprite->SetOx(w / 2);
		nametag_sprite->SetOy(h);
	}
}

void MultiplayerCharacter::OnSystemGraphicReady(FileRequestResult* result) 
{
	if (result && result->success) {
		UpdateNametagBitmap();
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
		UpdateNametagBitmap();
	}
}

void MultiplayerCharacter::SetNametag(const std::string& newnametag) 
{
	if (this->nametag != newnametag) {
		this->nametag = newnametag;
		UpdateNametagBitmap();
	}
}

void MultiplayerCharacter::MakeCharacter(unsigned int id) 
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

void MultiplayerCharacter::RemovePlayer(unsigned int id) 
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

std::shared_ptr<MultiplayerCharacter> MultiplayerCharacter::GetPlayer(unsigned int id) 
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

	sound.volume = sound.volume * MultiplayerCharacter::soundVolume * std::max(1.0f - l / MultiplayerCharacter::soundFalloffDistance, 0.0f);

	Main_Data::game_system->SePlay(sound);
}

std::map<unsigned int, std::shared_ptr<MultiplayerCharacter>> MultiplayerCharacter::characters = std::map<unsigned int, std::shared_ptr<MultiplayerCharacter>>();

float MultiplayerCharacter::soundVolume = 1.0f;
float MultiplayerCharacter::soundFalloffDistance = 10.0f;

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

void MultiplayerCharacter::OnMapDispose() 
{
	ClearCharacters();
}
