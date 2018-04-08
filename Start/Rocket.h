
#include "Actor.h"

class Rocket: public Actor
{
public:		
	Rocket(GameManager* pMan);

	~Rocket ();

	virtual void Update();
	virtual void Draw();
	virtual void Load();

	void IncAngle();  // rot right
	void DecAngle();  // rot left
	void Thrust();    // forward
	void Drift();    

	void setX(int x);
	void setY(int y);

	int safeLanding();  // check for safe landing

	int reset();
	int rest();

	float getH();
	float getW();

	float getVx();
	float getVy();

	int getFuel();

	void refuel(int amount);

	void  setExplosionStartTime(float time);
	float getExplosionStartTime();
	void  setExplosionElapsedTime(float time);
	float getExplosionElapsedTime() { return explosionElapsedTime;}

	void setThrusting(bool thrust);

	bool getThrusting() { return thrusting; }
	int getExplosionIndex() { return explosionIndex; }
	int getEngineIndex() { return engineIndex; }

	// ship screen location
	SDL_Rect rShip;
	SDL_Rect drawShip;
	SDL_Point center;
	SDL_Rect rFuelGage;
	float angle;
	bool landed;

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
	float thrusterForce;
	float shipMass;
	float thrust = thrusterForce / shipMass;
	float gravity;
	float vx;
	float vy;
	float ax;
	float ay;
	int fuel;
	float maxSpeedX;
	float maxSpeedY;
	int currentSprite;
	bool thrusting;

	int engineIndex;
	int explosionIndex;

	const Uint8 *keystates;
};