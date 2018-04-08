#include "stdafx.h"
#include "SDL.h"
#include "Rocket.h"
#include "Alien.h"
#include "GameManager.h"
#include "math.h"
#include "time.h"
#include "sound.h"


Rocket::Rocket(GameManager* pMan)  : Actor (pMan)
{
	reset();

	// add code to initialize variables in here
	rShip.w = 32;
	rShip.h = 64;
	rShip.x = 100;
	rShip.y = 10;
	center.x = rShip.w / 2;
	center.y = rShip.h / 2;
	drawShip.w = rShip.w;
	drawShip.h = rShip.h;
	
	thrusterForce = 1200;
	shipMass = 10;
	thrust = thrusterForce / shipMass;
	gravity = 15;
	vx = 0;
	vy = 0;
	ax = 0;
	ay = 0;
	angle = 0;
	fuel = 1000;
	maxSpeedX = 150;
	maxSpeedY = 300;
	landed = false;
	bool thrusting = false;

	rFuelGage.x = 10;
	rFuelGage.y = pMan->get_worldMaxY() - 110;
	rFuelGage.h = 100;
	rFuelGage.w = 20;

	keystates = SDL_GetKeyboardState(NULL);
}


Rocket::~Rocket(){}

int Rocket::reset()
{
	//   add code to reset rocket to start position 
	m_posX = 100;
	m_posY = 10;
	fuel = 1000;
	landed = false;
	rest();

	explosionElapsedTime = 0.f;

	return 1;
}

float Rocket::getH()
{
	return rShip.h;
}

float Rocket::getW()
{
	return rShip.w;
}

float Rocket::getVx()
{
	return vx;
}

float Rocket::getVy()
{
	return vy;
}

void Rocket::refuel(int amount)
{
	fuel += amount;

	if (fuel > 1000)
		fuel = 1000;
}

int Rocket::getFuel()
{
	return fuel;
}

void Rocket::setExplosionStartTime(float time)
{
	explosionStartTime = time;
}

float Rocket::getExplosionStartTime()
{
	return explosionStartTime;
}

void Rocket::setExplosionElapsedTime(float time)
{
	explosionElapsedTime = time;
}

void Rocket::setThrusting(bool thrust)
{
	thrusting = thrust;
}

int Rocket::rest()
{
	//  add code to set rocket velocity and angle to 0	
	vx = 0;
	vy = 0;
	ax = 0;
	ay = 0;
	angle = 0;

	return 1;
}

void Rocket::Load()
{
	//  add code to load rocket image
	sShip = SDL_LoadBMP("Sprites/my_ship.bmp");
	sShipThrust = SDL_LoadBMP("Sprites/my_shipThrust.bmp");
	SDL_SetColorKey(sShip, SDL_TRUE, 0x0000ff00);
	SDL_SetColorKey(sShipThrust, SDL_TRUE, 0x0000ff00);
	tShip = SDL_CreateTextureFromSurface(m_pGM->getRenderer(), sShip);
	tShipThrust = SDL_CreateTextureFromSurface(m_pGM->getRenderer(), sShipThrust);
	SDL_FreeSurface(sShip);  // we got the texture now -> free surface
	SDL_FreeSurface(sShipThrust);  // we got the texture now -> free surface

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

	engineIndex = LoadSound("Sounds/Thrust.wav", m_pGM);
	explosionIndex = LoadSound("Sounds/ShipExplosion.wav", m_pGM);

	//set to initial position
	reset();
}


void Rocket::Draw()
{
	//if (!m_Visible) return;

	switch(m_pGM->getGameState())
	{
	case INPLAY: // play

		rShip.x = (int) m_posX;
		rShip.y = (int) m_posY;
		drawShip.x = rShip.x + m_pGM->get_xOffset();
		drawShip.y = rShip.y + m_pGM->get_yOffset();

		//  add code to draw ship
		if(thrusting && fuel > 10)
			SDL_RenderCopyEx(m_pGM->getRenderer(), tShipThrust, NULL, &drawShip, angle, &center, SDL_FLIP_NONE);
		else
			SDL_RenderCopyEx(m_pGM->getRenderer(), tShip, NULL, &drawShip, angle, &center, SDL_FLIP_NONE);

		// draw fuelgage
		SDL_SetRenderDrawColor(m_pGM->getRenderer(), 0xF9, 0xF9, 0x09, 0xFF);
		SDL_RenderFillRect(m_pGM->getRenderer(), &rFuelGage);


		break;

	case FLASH: // explosion start
		m_pGM->clearTotalKills();

		//PlaySound(explosionIndex);
		explosionStartTime = m_pGM->getRunTime();
		
		// draw fuelgage
		SDL_SetRenderDrawColor(m_pGM->getRenderer(), 0xF9, 0xF9, 0x09, 0xFF);
		SDL_RenderFillRect(m_pGM->getRenderer(), &rFuelGage);
		
		break;
		
	case EXPLODING: // exploding

		explosionElapsedTime = m_pGM->getRunTime() - explosionStartTime;
		currentSprite = (int)(16 * explosionElapsedTime / 2);

		rExSourceRect.x = (currentSprite % 4) * rExSourceRect.w;
		if (currentSprite < 4)
			rExSourceRect.y = 0;
		else if(currentSprite>=4 && currentSprite< 8)
			rExSourceRect.y = rExSourceRect.h;
		else if(currentSprite>=8 && currentSprite< 12)
			rExSourceRect.y = 2 * rExSourceRect.h;
		else
			rExSourceRect.y = 3 * rExSourceRect.h;

		//grab ship location, adjust for explosion
		rExDestinationRect = drawShip;
		rExDestinationRect.x -= 40;
		rExDestinationRect.y -= 40;
		rExDestinationRect.h += 80;
		rExDestinationRect.w += 80;

		SDL_RenderCopyEx(m_pGM->getRenderer(), tExplosion, &rExSourceRect, &rExDestinationRect, 0, &centerExplosion, SDL_FLIP_NONE);

		// draw fuelgage
		SDL_SetRenderDrawColor(m_pGM->getRenderer(), 0xF9, 0xF9, 0x09, 0xFF);
		SDL_RenderFillRect(m_pGM->getRenderer(), &rFuelGage);

		break;

	case NOTHING: // nothing
		// draw fuelgage
		SDL_SetRenderDrawColor(m_pGM->getRenderer(), 0xF9, 0xF9, 0x09, 0xFF);
		SDL_RenderFillRect(m_pGM->getRenderer(), &rFuelGage);

		break;

	}

}

void Rocket::Update()
{
	elapsedTime = m_pGM->getElapsedTime();

	if (m_pGM->playerMovementBoost)
		thrusterForce = 2000.f;
	else
		thrusterForce = 1200.f;

	thrust = thrusterForce / shipMass;

	if(m_pGM->getGameState() == INPLAY || m_pGM->getGameState() == FLASH)
	{
		//  add code to update rocket position
		vx = vx + ax * elapsedTime;
		vy = vy + ay * elapsedTime + gravity * elapsedTime;
		
		if (vx > maxSpeedX)
			vx = maxSpeedX;
	
		if (vx < -maxSpeedX)
			vx = -maxSpeedX;
	
		if (vy > maxSpeedY)
			vy = maxSpeedY;
	
		if (vy < -maxSpeedY)
			vy = -maxSpeedY;
		
		if (!landed)
		{
			m_posX = m_posX + vx * elapsedTime;
			m_posY = m_posY + vy * elapsedTime;
		}
	}

	rFuelGage.y = -fuel / 10 + 680;
	rFuelGage.h = fuel * 97 / 1000 + 3;

	angle = fmod(angle,360.0);

}


void Rocket::IncAngle()
{
	//  add code to increment ship angle
	if (m_pGM->playerMovementBoost)
		angle += 2.5f;
	else
		angle += 1.5f;
}

void Rocket::DecAngle()
{
	//  add code to decrement ship angle
	if (m_pGM->playerMovementBoost)
		angle -= 2.5f;
	else
		angle -= 1.5f;
}

void Rocket::Thrust()
{
	//  add code to accelerate rocket
	if (landed)
		m_posY -= 1;

	if (fuel > 0)
	{
		ay = gravity - cos(M_PI * angle / 180.0f) * thrust;
		ax = sin(M_PI * angle / 180.0f) * thrust;

		if(!m_pGM->playerMovementBoost)
			fuel-=1;

		landed = false;
	}
}

void Rocket::Drift()
{
	//  update rocket ax/ay with no thrust
	ay = gravity;
	ax = 0;
}

void Rocket::setX(int x)
{
	m_posX = x;
}

void Rocket::setY(int y)
{
	m_posY = y;
}

int Rocket::safeLanding()
{
// return true if slow enough and good enough angle
	int result = 0;

	if (vx < 100 && vx > -100 && vy < 90 && vy > 0)
	{
		if (angle < 28 && angle > -28)
		{
			landed = true;
			result = 1;
		}
	}

	return result;
}