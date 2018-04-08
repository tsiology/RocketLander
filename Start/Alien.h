
#include "Actor.h"

class Alien: public Actor
{
public:		
	Alien(GameManager* pMan);

	~Alien ();

	virtual void Update();
	virtual void Draw();
	virtual void Load();

	enum aState {INPLAY,FLASH,EXPLODING,NOTHING};
	aState myState;

	void setX(int x);
	void setY(int y);

	int reset();
	int rest();
	int direction;

	bool inPlay;

	float getH();
	float getW();

	float getVx();
	float getVy();

	int healthPoints;
	bool destroyed;
	double lastTimeHit;

	void  setExplosionStartTime(float time);
	float getExplosionStartTime();
	void  setExplosionElapsedTime(float time);
	float getExplosionElapsedTime() { return explosionElapsedTime;}
	int getExplosionIndex() { return explosionIndex; }
	int getEngineIndex() { return engineIndex; }


	// ship screen location
	SDL_Rect rShip;
	SDL_Rect drawShip;
	SDL_Point center;

	void setStateNothing();

private:
	double elapsedTime;
	double explosionStartTime;
	double explosionElapsedTime;

	// image pointers
	SDL_Surface* sShip;
	SDL_Texture* tShip;
	SDL_Surface* sShipThrust;
	SDL_Texture* tShipThrust;
	SDL_Surface* sExplosion;
	SDL_Texture* tExplosion;

	
	SDL_Rect rExSourceRect;
	SDL_Rect rExDestinationRect;
	SDL_Point centerExplosion;

	//movement variables
	float vx;
	float vy;
	int currentSprite;

	int engineIndex;
	int explosionIndex;

	const Uint8 *keystates;
};