//Debris will populate the screen acting as obstacles for the player
#include "Actor.h"

class Debris : public Actor
{
public:
	Debris(GameManager* pMan);

	~Debris();

	virtual void Update();
	virtual void Draw();
	virtual void Load();
	
	//set this debris to inPlay with starting XY position and Velocity
	void spawnDebris(int xPos, int yPos, int xVelolcity, int yVelocity);

	void setX(int x);
	void setY(int y);
	void reset();
	int rest();

	float getH();
	float getW();
	float getVx();
	float getVy();

	int xDirection;
	int yDirection;
	bool inPlay;

	// Debris screen location
	SDL_Rect rDebris;
	SDL_Rect drawDebris;
	SDL_Point center;

private:
	double elapsedTime;
	
	// image pointers
	SDL_Surface* sDebris;
	SDL_Texture* tDebris;
	
	//movement variables
	float vx;
	float vy;
};