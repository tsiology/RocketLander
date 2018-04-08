// Actor represents the most basic form of an on screen game object. 
// All on screen game objects inherit from actor.
#ifndef ACTORH
#define ACTORH

class GameManager;

class Actor
{
public:		
	Actor();
	Actor(GameManager* pMan);

	virtual ~Actor ();

	// pure virtual functions must be implemented by derived class
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Load() = 0;

	// functions for getting the object's position or offset
	float X() { return m_posX;}
	float Y() { return m_posY;}
	float OX() { return m_offX;}
	float OY() { return m_offY;}

protected:
	GameManager* m_pGM;  // pointer to game manager

	// object world position
	float m_posX;
	float m_posY;

	// object world offset
	float m_offX;
	float m_offY;
};

#endif  #ifndef ACTORH