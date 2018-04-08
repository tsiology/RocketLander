//This class uses parallel arrays to track, draw, and update all on in game projectiles.
#include "stdafx.h"
#include "SDL.h"
#include "Rocket.h"
#include "Alien.h"
#include "GameManager.h"
#include "MissileManager.h"
#include "math.h"
#include "time.h"
#include "sound.h"


MissileManager::MissileManager(GameManager* pMan) : Actor(pMan)
{	
	for (int i = 0; i < MAXMISSILES; i++)
	{
		// screen location
		rMissiles[i].w = 8;
		rMissiles[i].h = 16;

		//draw location is screen location + world offset
		drawMissiles[i].w = rMissiles[i].w;
		drawMissiles[i].h = rMissiles[i].h;

		centerMissiles[i].x = rMissiles[i].w / 2;
		centerMissiles[i].y = rMissiles[i].h / 2;

		//movement variables
		vxMissiles[i] = 0.f;
		vyMissiles[i] = 0.f;
		axMissiles[i] = 0.f;
		ayMissiles[i] = 0.f;
		angleMissiles[i] = 0.f;
		visibleMissiles[i] = false;
		startTimeMissiles[i] = 0;
	}

	maxSpeedX = 400.f;
	maxSpeedY = 400.f;
	playerMissileRate = 1.5f;
	alienMissileRate = 4.f;
	playerLastFireTime = 0.f;
	alienLastFireTime = 8.f; //init to 8 to effect aliens first missile fire time
	missilesInPlay = 0;
	keystates = SDL_GetKeyboardState(NULL);
	missileThrust = 200.f;
	missileLifeSpan = 6.f;
}


MissileManager::~MissileManager() {}

void MissileManager::resetMan()
{
	playerLastFireTime = 0.f;
	alienLastFireTime = 8.f;
	missilesInPlay = 0;
}

int MissileManager::reset(int i)
{
	//   add code to reset missile 
	vxMissiles[i] = 0.f;
	vyMissiles[i] = 0.f;
	axMissiles[i] = 0.f;
	ayMissiles[i] = 0.f;
	angleMissiles[i] = 0.f;
	visibleMissiles[i] = false;
	explodingMissiles[i] = false;

	startTimeMissiles[i] = 0;
	explosionStartTime[i] = 0;

	return 1;
}

//attempt to spawn missile as either the player or alien's projectile
bool MissileManager::launchMissile(bool player)
{
	bool result = false;

	if(player)
	{
		if (missilesInPlay < MAXMISSILES && (m_pGM->getRunTime() - playerLastFireTime) > playerMissileRate)
		{
			int index = 0;
			//find first open index in parallel arrays
			for (index; index <= missilesInPlay; index++)
			{
				if (!visibleMissiles[index] && !explodingMissiles[index])
					break;
			}

			playerFriendlyMissiles[index] = true;

			vxMissiles[index] = 0.f;
			vyMissiles[index] = 0.f;
			axMissiles[index] = 0.f;
			ayMissiles[index] = 0.f;
			playerLastFireTime = m_pGM->getRunTime();
			startTimeMissiles[index] = playerLastFireTime;
			pxMissiles[index] = m_pGM->m_pRocket->rShip.x + m_pGM->m_pRocket->center.x;
			pyMissiles[index] = m_pGM->m_pRocket->rShip.y + m_pGM->m_pRocket->center.y;
			drawMissiles[index].x = m_pGM->m_pRocket->drawShip.x;
			drawMissiles[index].y = m_pGM->m_pRocket->drawShip.y;
			angleMissiles[index] = m_pGM->m_pRocket->angle;
			visibleMissiles[index] = true;
			++missilesInPlay;

			result = true;
		}
	}//end player missile

	//alien missile
	else
	{
		if (missilesInPlay < MAXMISSILES && (m_pGM->getRunTime() - alienLastFireTime) > alienMissileRate)
		{
			int index = 0;
			//find first open index in parallel arrays
			for (index; index <= missilesInPlay; index++)
			{
				if (!visibleMissiles[index] && !explodingMissiles[index])
					break;
			}

			playerFriendlyMissiles[index] = false;

			vxMissiles[index] = 0.f;
			vyMissiles[index] = 0.f;
			axMissiles[index] = 0.f;
			ayMissiles[index] = 0.f;
			alienLastFireTime = m_pGM->getRunTime();
			startTimeMissiles[index] = alienLastFireTime;
			pxMissiles[index] = m_pGM->m_pAlien->rShip.x + m_pGM->m_pAlien->center.x;
			pyMissiles[index] = m_pGM->m_pAlien->rShip.y + m_pGM->m_pAlien->center.y;
			drawMissiles[index].x = m_pGM->m_pAlien->drawShip.x;
			drawMissiles[index].y = m_pGM->m_pAlien->drawShip.y;

			if(m_pGM->m_pAlien->X() < m_pGM->m_pRocket->X())
				angleMissiles[index] = 85.f;
			else
				angleMissiles[index] = -86.f;

			visibleMissiles[index] = true;
			++missilesInPlay;

			result = true;

			//printf("Player fired a missile, count it %d\n", MissilesInPlay);
		}
	}
	return result;
}

void MissileManager::Load()
{
	//  add code to load  image
	sMissiles = SDL_LoadBMP("Sprites/my_missile.bmp");
	SDL_SetColorKey(sMissiles, SDL_TRUE, 0x0000ff00);
	tMissiles = SDL_CreateTextureFromSurface(m_pGM->getRenderer(), sMissiles);
	SDL_FreeSurface(sMissiles);  // we got the texture now -> free surface
	
	a_sMissiles = SDL_LoadBMP("Sprites/alien_missile.bmp");
	SDL_SetColorKey(a_sMissiles, SDL_TRUE, 0x0000ff00);
	a_tMissiles = SDL_CreateTextureFromSurface(m_pGM->getRenderer(), a_sMissiles);
	SDL_FreeSurface(a_sMissiles);  // we got the texture now -> free surface

	missileFireIndex = LoadSound("Sounds/MissileFire.wav", m_pGM);
	missileHitIndex = LoadSound("Sounds/MissileHit.wav", m_pGM);
	alienFireIndex = LoadSound("Sounds/AlienMissile.wav", m_pGM);


	// load explosion image
	sExplosion = SDL_LoadBMP("Sprites/explosion_sheet.bmp");
	SDL_SetColorKey(sExplosion, SDL_TRUE, 0x0000ff00);
	tExplosion = SDL_CreateTextureFromSurface(m_pGM->getRenderer(), sExplosion);
	SDL_FreeSurface(sExplosion);  // we got the texture now -> free surface
	
	for (int i = 0; i < MAXMISSILES; i++)
	{
		rExSourceRect[i].x = 0;
		rExSourceRect[i].y = 0;
		rExSourceRect[i].w = sExplosion->w / 4;
		rExSourceRect[i].h = sExplosion->h / 4;
		centerExplosion[i].x = rExSourceRect[i].w / 4 / 2;
		centerExplosion[i].y = rExSourceRect[i].y / 4 / 2;
	}

}

void MissileManager::Draw()
{
	gState = m_pGM->getGameState();

	for (int i = 0; i < MAXMISSILES; i++)
	{
		if (visibleMissiles[i] == true)
		{
			drawMissiles[i].x = pxMissiles[i] + m_pGM->get_xOffset();
			drawMissiles[i].y = pyMissiles[i] + m_pGM->get_yOffset();

			if(playerFriendlyMissiles[i])
				SDL_RenderCopyEx(m_pGM->getRenderer(), tMissiles, NULL, &drawMissiles[i], angleMissiles[i], &centerMissiles[i], SDL_FLIP_NONE);
			else
				SDL_RenderCopyEx(m_pGM->getRenderer(), a_tMissiles, NULL, &drawMissiles[i], angleMissiles[i], &centerMissiles[i], SDL_FLIP_NONE);
		}
		else if (explodingMissiles[i] == true)
		{
			explosionElapsedTime[i] = m_pGM->getRunTime() - explosionStartTime[i];
			currentSprite[i] = (int)(16 * explosionElapsedTime[i] / 2);

			if(explosionElapsedTime[i] < 1.8f)
			{
				rExSourceRect[i].x = (currentSprite[i] % 4) * rExSourceRect[i].w;
				if (currentSprite[i] < 4)
					rExSourceRect[i].y = 0;
				else if (currentSprite[i] >= 4 && currentSprite[i]< 8)
					rExSourceRect[i].y = rExSourceRect[i].h;
				else if (currentSprite[i] >= 8 && currentSprite[i]< 12)
					rExSourceRect[i].y = 2 * rExSourceRect[i].h;
				else
					rExSourceRect[i].y = 3 * rExSourceRect[i].h;

				rExDestinationRect[i] = drawMissiles[i];
				rExDestinationRect[i].x -= 40;
				rExDestinationRect[i].y -= 40;
				rExDestinationRect[i].h += 80;
				rExDestinationRect[i].w += 80;

				SDL_RenderCopyEx(m_pGM->getRenderer(), tExplosion, &rExSourceRect[i], &rExDestinationRect[i], 0, &centerExplosion[i], SDL_FLIP_NONE);
			}

			else
			{
				explodingMissiles[i] == false;
			}
		}

	}


}

void MissileManager::Update()
{
	elapsedTime = m_pGM->getElapsedTime();
	runTime = m_pGM->getRunTime();

	if (m_pGM->playerWeaponBoost)
		playerMissileRate = 0.75f;
	else
		playerMissileRate = 1.5f;

	for (int i = 0; i < MAXMISSILES; i++)
	{
		if (visibleMissiles[i] == true)
		{
			if (runTime - startTimeMissiles[i] > missileLifeSpan)
			{
				reset(i);
				--missilesInPlay;
			}
			else
			{
				ayMissiles[i] = -cos(M_PI * angleMissiles[i] / 180.0f) * missileThrust;
				axMissiles[i] = sin(M_PI * angleMissiles[i] / 180.0f) * missileThrust;
				vxMissiles[i] = vxMissiles[i] + axMissiles[i] * elapsedTime;
				vyMissiles[i] = vyMissiles[i] + ayMissiles[i] * elapsedTime + 15 * elapsedTime;
				pxMissiles[i] = pxMissiles[i] + vxMissiles[i] * elapsedTime;
				pyMissiles[i] = pyMissiles[i] + vyMissiles[i] * elapsedTime;
			}

			if (vxMissiles[i] > maxSpeedX)
				vxMissiles[i] = maxSpeedX;

			if (vyMissiles[i] > maxSpeedY)
				vyMissiles[i] = maxSpeedY;

			if (vxMissiles[i] < -maxSpeedX)
				vxMissiles[i] = -maxSpeedX;

			if (vyMissiles[i] < -maxSpeedY)
				vyMissiles[i] = -maxSpeedY;

		}

	}

}

