#include "game_player.h"
#include "game_party.h"
#include "sprite_character.h"
#include "packets/actorinfopacket.h"
#include <map>
#include <set>
#include <queue>


class MultiplayerCharacter : public Game_PlayerBase
{
	private:
	unsigned int id;
	std::string nametag;
	std::string systemname;
	std::queue<std::pair<int, int>> moveQueue;
	int flashR, flashG, flashB, flashP, flashT;
	Game_Actor* actors[ACTOR_SLOTS];
	public:
	std::unique_ptr<Sprite_Character> sprite;
	MultiplayerCharacter(unsigned int id);

	static void MakeCharacter(unsigned int id);

	void MultiplayerMove(int x, int y);

	void UpdateNextMovementAction() override;

	void Update();

	float GetDistanceToPlayer();

	void PlaySound(lcf::rpg::Sound sound);

	void DisplayNametag(BitmapRef& surface, int zoom);

	void UpdateActor(IActorInfoPacket* packet, int actorIndex);

	inline void SetFlash(int r, int g, int b, int power, int frames) 
	{
		flashR = r;
		flashG = g;
		flashB = b;
		flashP = power;
		flashT = frames;
	}

	inline void SetSystem(const std::string& systemname) { this->systemname = systemname; }

	inline void SetNametag(const std::string& nametag) { this->nametag = nametag; }

	inline const std::string& GetNametag() const { return this->nametag; }

	static void UpdateCharacters();

	static void FullresPass(BitmapRef& surface, int zoom);
	
	static void ClearCharacters();
	
	static void RemovePlayer(unsigned int id);

	std::unique_ptr<Game_Party>& GetPlayerParty();
	std::set<unsigned int> GetMyCoopParty();
	
	static std::shared_ptr<MultiplayerCharacter> GetPlayer(unsigned int id);
	
	static std::map<unsigned int, std::shared_ptr<MultiplayerCharacter>> characters;

	static std::vector<Game_Actor*> GetAllParties();

	static void OnMapDispose();

	static float soundFalloffDistance;
	static float soundVolume;
};