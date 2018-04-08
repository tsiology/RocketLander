//Debris will populate the screen acting as obstacles for the player
#include "stdafx.h"
#include "SDL.h"
#include "Rocket.h"
#include "GameManager.h"
#include "Debris.h"

Debris::Debris(GameManager* pMan) : Actor(pMan)
{
	// add code to initialize variables in here
	rDebris.w = 128;
	rDebris.h = 64;
	rDebris.x = 0;
	rDebris.y = 0;
	center.x = rDebris.w / 2;
	center.y = rDebris.h / 2;
	drawDebris.w = rDebris.w;
	drawDebris.h = rDebris.h;
	vx = 0;
	vy = 0;
	inPlay = false;
}

Debris::~Debris() {}

void Debris::reset()
{
	inPlay = false;
}

//set this debris to inPlay with starting XY position and Velocity
void Debris::spawnDebris(int x, int y, int vxIn, int vyIn)
{
	inPlay = true;
	m_posX = x;
	m_posY = y;
	vx = vxIn;
	vy = vyIn;

	//direction will depend on starting position
	if (x > m_pGM->get_worldMaxX() / 2.0f)
		xDirection = -1;
	else
		xDirection = 1;

	if (y > m_pGM->get_worldMaxY() / 2.0f)
		yDirection = -1;
	else
		yDirection = 1;
}

float Debris::getH()
{
	return rDebris.h;
}

float Debris::getW()
{
	return rDebris.w;
}

float Debris::getVx()
{
	return vx;
}

float Debris::getVy()
{
	return vy;
}

int Debris::rest()
{
	vx = 0;
	vy = 0;

	return 1;
}

void Debris::setX(int x)
{
	m_posX = x;
}

void Debris::setY(int y)
{
	m_posY = y;
}

void Debris::Load()
{
	sDebris = SDL_LoadBMP("Sprites/debris.bmp");
	SDL_SetColorKey(sDebris, SDL_TRUE, 0x0000ff00);
	tDebris = SDL_CreateTextureFromSurface(m_pGM->getRenderer(), sDebris);
	SDL_FreeSurface(sDebris);  // we got the texture now -> free surface
}


void Debris::Draw()
{
	if (inPlay)
	{
		rDebris.x = (int)m_posX;
		rDebris.y = (int)m_posY;
		drawDebris.x = rDebris.x + m_pGM->get_xOffset();
		drawDebris.y = rDebris.y + m_pGM->get_yOffset();

		SDL_RenderCopyEx(m_pGM->getRenderer(), tDebris, NULL, &drawDebris, 0, &center, SDL_FLIP_NONE);
	}
}

void Debris::Update()
{
	if (inPlay)
	{
		elapsedTime = m_pGM->getElapsedTime();
		m_posX = m_posX + xDirection * vx * elapsedTime;
		m_posY = m_posY + yDirection * vy * elapsedTime;

		//check for collision with player, change gameState to FLASH is so
		if (m_pGM->getGameState()==INPLAY && m_pGM->m_pRocket->drawShip.x > drawDebris.x + 10 && m_pGM->m_pRocket->drawShip.x < drawDebris.x + drawDebris.w - 10)
		{
			if (m_pGM->m_pRocket->drawShip.y > drawDebris.y + 10 && m_pGM->m_pRocket->drawShip.y < drawDebris.y + drawDebris.h - 20)
			{
				m_pGM->setFlash();
			}
		}
	}

	//nothing to do if not in play
}
