#pragma once
#include "game_player.h"
#include "game_party.h"
#include "sprite.h"
#include "sprite_character.h"
#include "packets/actorinfopacket.h"
#include "async_handler.h"
#include <map>
#include <set>
#include <queue>
#include <string>
#include <memory>
#include "textbubble.h"

class MultiplayerCharacter : public Game_PlayerBase
{
private:
	unsigned int id;
	std::string nametag;
	std::string systemname;
	std::queue<std::pair<int, int>> moveQueue;
	int flashR, flashG, flashB, flashP, flashT;
	Game_Actor* actors[ACTOR_SLOTS];
    std::vector<std::unique_ptr<TextBubble>> text_bubbles;

	FileRequestBinding system_request_id;

	void OnSystemGraphicReady(FileRequestResult* result);

public:
	std::unique_ptr<Sprite_Character> sprite;
	std::unique_ptr<Sprite> nametag_sprite;

	MultiplayerCharacter(unsigned int id);
	~MultiplayerCharacter() override;

	static void MakeCharacter(unsigned int id);

	void MultiplayerMove(int x, int y);

	void UpdateNextMovementAction() override;

	void Update();

	float GetDistanceToPlayer();

	void PlaySound(lcf::rpg::Sound sound);

	void UpdateActor(IActorInfoPacket* packet, int actorIndex);

	inline void SetFlash(int r, int g, int b, int power, int frames) 
	{
		flashR = r;
		flashG = g;
		flashB = b;
		flashP = power;
		flashT = frames;
	}

	void SetSystem(const std::string& systemname);
	inline const std::string& GetSystem() const { return this->systemname; }

	void SetNametag(const std::string& nametag);
	inline const std::string& GetNametag() const { return this->nametag; }

	static void UpdateCharacters();
	static void ClearCharacters();
	static void RemovePlayer(unsigned int id);

	std::unique_ptr<Game_Party>& GetPlayerParty();
	std::set<unsigned int> GetMyCoopParty();
	
	static std::shared_ptr<MultiplayerCharacter> GetPlayer(unsigned int id);
	
	static std::map<unsigned int, std::shared_ptr<MultiplayerCharacter>> characters;

	static std::vector<Game_Actor*> GetAllParties();

	static void OnMapDispose();
	void CreateMessageBubble(const std::string text);
};
