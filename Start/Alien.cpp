#include "stdafx.h"
#include "SDL.h"
#include "Alien.h"
#include "Rocket.h"
#include "GameManager.h"
#include "math.h"
#include "time.h"
#include "sound.h"


Alien::Alien(GameManager* pMan)  : Actor (pMan)
{
	reset();

	// add code to initialize variables in here
	rShip.w = 64;
	rShip.h = 32;
	rShip.x = -100;
	rShip.y = 10;
	center.x = rShip.w / 2;
	center.y = rShip.h / 2;
	drawShip.w = rShip.w;
	drawShip.h = rShip.h;
	vx = 0;
	vy = 0;
	inPlay = false;
	myState = NOTHING;

	direction = 1;

	healthPoints = 2;
	destroyed = false;
	lastTimeHit = 0.f;

	keystates = SDL_GetKeyboardState(NULL);

}


Alien::~Alien(){}

int Alien::reset()
{
	//   add code to reset rocket to start position 
	m_posX = -100;
	m_posY = 10;

	inPlay = false;
	myState = NOTHING;
	direction = 1;

	healthPoints = 2;
	destroyed = false;
	lastTimeHit = 0.f;

	explosionElapsedTime = 0.f;

	rest();

	return 1;
}

float Alien::getH()
{
	return rShip.h;
}

float Alien::getW()
{
	return rShip.w;
}

float Alien::getVx()
{
	return vx;
}

float Alien::getVy()
{
	return vy;
}

void Alien::setExplosionStartTime(float time)
{
	explosionStartTime = time;
}

float Alien::getExplosionStartTime()
{
	return explosionStartTime;
}

void Alien::setExplosionElapsedTime(float time)
{
	explosionElapsedTime = time;
}

int Alien::rest()
{
	//  add code to set rocket velocity and angle to 0	
	vx = 0;
	vy = 0;

	return 1;
}

void Alien::setStateNothing()
{
	myState = NOTHING;
}

void Alien::Load()
{
	//  add code to load rocket image
	sShip = SDL_LoadBMP("Sprites/alien_ship.bmp");
	SDL_SetColorKey(sShip, SDL_TRUE, 0x0000ff00);
	tShip = SDL_CreateTextureFromSurface(m_pGM->getRenderer(), sShip);
	SDL_FreeSurface(sShip);  // we got the texture now -> free surface
	
	// load explosion image
	sExplosion = SDL_LoadBMP("Sprites/explosion_sheet.bmp");
	SDL_SetColorKey(sExplosion, SDL_TRUE, 0x0000ff00);
	tExplosion = SDL_CreateTextureFromSurface(m_pGM->getRenderer(), sExplosion);
	SDL_FreeSurface(sExplosion);  // we got the texture now -> free surface

	rExSourceRect.x = 0;
	rExSourceRect.y = 0;
	rExSourceRect.w = sExplosion->w / 4;
	rExSourceRect.h = sExplosion->h / 4;
	centerExplosion.x = rExSourceRect.w / 4 / 2;
	centerExplosion.y = rExSourceRect.y / 4 / 2;

	//explosionIndex = LoadSound("explosion-01.wav", m_pGM);
	explosionIndex = LoadSound("Sounds/ShipExplosion.wav", m_pGM);
	engineIndex = LoadSound("Sounds/AlienShip.wav", m_pGM);

	//set to initial position
	reset();
}


void Alien::Draw()
{
	if(inPlay)
	{
		rShip.x = (int) m_posX;
		rShip.y = (int) m_posY;
		drawShip.x = rShip.x + m_pGM->get_xOffset();
		drawShip.y = rShip.y + m_pGM->get_yOffset();

		//  add code to draw ship
		SDL_RenderCopyEx(m_pGM->getRenderer(), tShip, NULL, &drawShip, 0, &center, SDL_FLIP_NONE);
		
		//grab ship location, adjust for explosion
		rExDestinationRect = drawShip;
		rExDestinationRect.x -= 40;
		rExDestinationRect.y -= 40;
		rExDestinationRect.h += 80;
		rExDestinationRect.w += 80;
	}

	else if (myState == EXPLODING)
	{
		explosionElapsedTime = m_pGM->getRunTime() - explosionStartTime;
		currentSprite = (int)(16 * explosionElapsedTime / 2);

		rExSourceRect.x = (currentSprite % 4) * rExSourceRect.w;
		if (currentSprite < 4)
			rExSourceRect.y = 0;
		else if (currentSprite >= 4 && currentSprite< 8)
			rExSourceRect.y = rExSourceRect.h;
		else if (currentSprite >= 8 && currentSprite< 12)
			rExSourceRect.y = 2 * rExSourceRect.h;
		else
			rExSourceRect.y = 3 * rExSourceRect.h;


		SDL_RenderCopyEx(m_pGM->getRenderer(), tExplosion, &rExSourceRect, &rExDestinationRect, 0, &centerExplosion, SDL_FLIP_NONE);
	}
}

void Alien::Update()
{
	elapsedTime = m_pGM->getElapsedTime();
	

	if (myState == FLASH)
	{
		m_pGM->incTotalKills();
		explosionStartTime = m_pGM->getRunTime();
		myState = EXPLODING;
		PlaySound(getExplosionIndex());
	}

	//  add code to update rocket position
	vx = 80;
		
	if (m_pGM->m_pRocket->Y() > m_posY + 10)
	{
		vy = 40;
	}
	else if (m_pGM->m_pRocket->Y() < m_posY - 10)
	{
		vy = -40;
	}
	else
		vy = 0;
		

	if (inPlay)
	{
		m_posX = m_posX + direction * vx * elapsedTime;
		m_posY = m_posY + vy * elapsedTime;
	}
	//keep off screen until Alien enters play
	else
	{
		m_posX = -100;
		m_posY = m_pGM->m_pRocket->Y();
	}

	//change direction at edge of screen
	if (direction == 1 && m_posX + rShip.w > m_pGM->get_worldMaxX())
	{
		direction = -1;
	}
	else if (direction == -1 && m_posX < 0)
	{
		direction = 1;
	}
	
	if (healthPoints <= 0 && inPlay)
	{
		inPlay = false;
		destroyed = true;
		myState = FLASH;

	}

}

void Alien::setX(int x)
{
	m_posX = x;
}

void Alien::setY(int y)
{
	m_posY = y;
}
