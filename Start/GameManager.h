
#include "time.h"
#include "SDL.h"
#include <ctime>
#include "SDL_audio.h"
#include <SDL_ttf.h>
#include <string>
#include <stdlib.h>

class Rocket;
class Alien;
class Actor;
class Pad;
class MissileManager;
class Debris;

enum gameStateType { INPLAY, FLASH, EXPLODING, NOTHING, LANDED };

#define MAXLAND   30
#define MAXDEBRIS 10

class GameManager
{
public:		
	GameManager();

	~GameManager ();

	int Initialize(Actor* Actors[]);

	void Update();
	void Draw();

	int padIntersect(Pad* pPad, Rocket* pRocket);

	void UserInput();

	gameStateType getGameState() { return m_gameState;}

	clock_t getCurrTime() { return m_currTime;}
	double getElapsedTime() { return m_elapsedTime;}
	double getRunTime() { return m_runTime; }

	double lastKillTime;
	double alienSpawnTime;
	double resetTime;
	int initializeSDL();

	int endGraphics();

	int gameOver();

	SDL_Renderer* getRenderer();

	float get_worldMaxY();
	float get_worldMaxX();
	float get_xOffset();
	float get_yOffset();
	float get_margin();

	void update_xOffset( );
	void update_yOffset( );

	SDL_AudioSpec getAudioSpec() { return obtained; };

	Rocket* m_pRocket;
	Alien* m_pAlien;
	MissileManager* m_Missiles;

	TTF_Font* font;
	SDL_Surface* sText;
	SDL_Texture* tText_UFO;
	SDL_Texture* tText_Clear;
	SDL_Texture* tText_Continue;
	SDL_Texture* tText_Speed;
	SDL_Texture* tText_Angle;
	SDL_Texture* tText_Altitude;
	SDL_Texture* tText_Restart;


	SDL_Rect     rText;

	int waveCount;
	bool waveComplete;
	int waveKills;
	int totalKills;
	int weaponBoostPad;
	int movementBoostPad;

	bool playerWeaponBoost;
	bool playerMovementBoost;

	double wBoostStartTime;
	double mBoostStartTime;

	double wBoostDuration;
	double mBoostDuration;


	void displayText_UFO();
	void displayText_Clear();
	void displayText_Continue();
	void displayText_Speed();
	void displayText_Angle();
	void displayText_Altitude();
	void displayText_Restart();

	void incTotalKills();
	void clearTotalKills();
	void setFlash();

private:

	SDL_AudioSpec obtained;
	SDL_Window* window;
	SDL_Renderer* renderer;
	const Uint8 *keystates;
	SDL_Event event;
	SDL_Color redFontColor;
	gameStateType m_gameState; //  { INPLAY, FLASH, EXPLODING, NOTHING, LANDED };

	
	// time vairiables
	clock_t m_currTime, m_oTime, m_eTime, m_fTime;
	clock_t m_exploTime;
	double m_elapsedTime;
	double m_runTime;
	long int m_iFrame;
	float tpf;
	double thrustSoundTime;
	double alienEngineTime;
	
	bool playingThrust;
	int m_screenWidth;
	int m_screenHeight;
	float m_worldMaxX;
	float m_worldMaxY;

	float m_xOffset;
	float m_yOffset;
	float m_margin;

	Pad* m_Pads[MAXLAND];  // landing pads
	int m_numPads;

	Debris* m_Debris[MAXDEBRIS];  // floating space debris
	int m_numDebris;
	int nActors;


	int done;


};