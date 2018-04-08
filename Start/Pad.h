//Landing pads act as refueling stations and may also grant boosts
#include "Actor.h"

class Pad: public Actor
{
public:		
	Pad(GameManager* pMan, float x, float y, float h, float w);

	~Pad ();

	virtual void Update();
	virtual void Draw();
	virtual void Load();

	float getH();
	float getW();
	bool getFuel();
	void setFuel(bool val);

	bool weaponBoost;
	bool movementBoost;

	void reset();

private:
	
	SDL_Rect rPad;
	SDL_Rect drawPad;
	SDL_Rect drawPadArt;

	SDL_Surface* sPad;
	SDL_Texture* tPad;
	
	//weapon boosting pad
	SDL_Surface* sPad_wBoost;
	SDL_Texture* tPad_wBoost;

	//movement boosting pad
	SDL_Surface* sPad_mBoost;
	SDL_Texture* tPad_mBoost;

	bool fueledStation;
};