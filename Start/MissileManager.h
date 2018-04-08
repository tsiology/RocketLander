//This class uses parallel arrays to track, draw, and update all on in game projectiles.
#include "Actor.h"

#define MAXMISSILES 10

class MissileManager : public Actor
{
public:
	MissileManager(GameManager* pMan);

	~MissileManager();

	virtual void Update();
	virtual void Draw();
	virtual void Load();

	// image pointers
	SDL_Surface* sMissiles;
	SDL_Texture* tMissiles;
	SDL_Surface* sExplosion;
	SDL_Texture* tExplosion;

	// alien image pointers
	SDL_Surface* a_sMissiles;
	SDL_Texture* a_tMissiles;

	// screen location
	SDL_Rect rMissiles[MAXMISSILES];
	SDL_Rect drawMissiles[MAXMISSILES];
	SDL_Rect rExSourceRect[MAXMISSILES];  //explosion source rectangles
	SDL_Rect rExDestinationRect[MAXMISSILES];

	SDL_Point centerMissiles[MAXMISSILES];
	SDL_Point centerExplosion[MAXMISSILES];

	//movement variables
	float pxMissiles[MAXMISSILES];
	float pyMissiles[MAXMISSILES];
	float vxMissiles[MAXMISSILES];
	float vyMissiles[MAXMISSILES];
	float axMissiles[MAXMISSILES];
	float ayMissiles[MAXMISSILES];
	float angleMissiles[MAXMISSILES];
	double explosionStartTime[MAXMISSILES];
	double explosionElapsedTime[MAXMISSILES];
	bool playerFriendlyMissiles[MAXMISSILES];
	bool  visibleMissiles[MAXMISSILES];
	bool  explodingMissiles[MAXMISSILES];
	double startTimeMissiles[MAXMISSILES];
	int currentSprite[MAXMISSILES];
	
	float maxSpeedX;
	float maxSpeedY;

	void resetMan();
	int reset(int index);

	bool launchMissile(bool player);
	int getMissileFireIndex() { return missileFireIndex; }
	int getMissileHitIndex() { return missileHitIndex; }
	int getAlienFireIndex() { return alienFireIndex; }

	float playerMissileRate;
	float alienMissileRate;
	double playerLastFireTime;
	double alienLastFireTime;

	int missilesInPlay;
	float missileThrust;
	double missileLifeSpan;

private:
	double elapsedTime;
	const Uint8 *keystates;
	int missileFireIndex;
	int missileHitIndex;
	int alienFireIndex;
	gameStateType gState;
	double runTime;
};