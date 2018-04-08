#include "stdafx.h"
#include "SDL.h"
#include "GameManager.h"
#include "time.h"
#include "Rocket.h"
#include "Alien.h"
#include "Background.h"
#include "Pad.h"
#include <stdio.h>
#include "sound.h"
#include "MissileManager.h"
#include "Debris.h"
#include <cstdlib>

GameManager::GameManager(){}
GameManager::~GameManager(){}


int GameManager::Initialize(Actor* Actors[])
{  // create actors used in the game.  Init screen size, time, and SDL
	
	waveCount = 1;
	waveComplete = false;
	waveKills = 0;
	totalKills = 0;
	lastKillTime = 0.f;
	alienSpawnTime = 4.f;

	nActors = 0;
	m_numPads = 0;
	m_numDebris = 0;

	// create background
	Actors[nActors++] = (Actor*) new Background(this);

	// create rocket
	m_pRocket = new Rocket(this);
	Actors[nActors++] = (Actor*) m_pRocket;

	// create alien
	m_pAlien = new Alien(this);
	Actors[nActors++] = (Actor*)m_pAlien;

	// create missle manager
	m_Missiles = new MissileManager(this);
	Actors[nActors++] = (Actor*)m_Missiles;

	// create landing pads
	float padH = 12.f;
	float padW = 80.f;
	m_Pads[m_numPads++] = new Pad(this, 220.0, 350.0, padH,padW);
	Actors[nActors++] = (Actor*) m_Pads[m_numPads -1];
	m_Pads[m_numPads++] = new Pad(this, 440.0, 450.0, padH, padW);
	Actors[nActors++] = (Actor*) m_Pads[m_numPads -1];
	m_Pads[m_numPads++] = new Pad(this, 700.0, 300.0, padH, padW);
	Actors[nActors++] = (Actor*)m_Pads[m_numPads - 1];
	m_Pads[m_numPads++] = new Pad(this, 1020.0, 350.0, padH, padW);
	Actors[nActors++] = (Actor*)m_Pads[m_numPads - 1];
	m_Pads[m_numPads++] = new Pad(this, 1240.0, 450.0, padH, padW);
	Actors[nActors++] = (Actor*)m_Pads[m_numPads - 1];
	m_Pads[m_numPads++] = new Pad(this, 1500.0, 500.0, padH, padW);
	Actors[nActors++] = (Actor*)m_Pads[m_numPads - 1];
	m_Pads[m_numPads++] = new Pad(this, 340.0, 850.0, padH, padW);
	Actors[nActors++] = (Actor*)m_Pads[m_numPads - 1];
	m_Pads[m_numPads++] = new Pad(this, 560.0, 950.0, padH, padW);
	Actors[nActors++] = (Actor*)m_Pads[m_numPads - 1];
	m_Pads[m_numPads++] = new Pad(this, 820.0, 800.0, padH, padW);
	Actors[nActors++] = (Actor*)m_Pads[m_numPads - 1];
	m_Pads[m_numPads++] = new Pad(this, 1000.0, 850.0, padH, padW);
	Actors[nActors++] = (Actor*)m_Pads[m_numPads - 1];
	m_Pads[m_numPads++] = new Pad(this, 1200.0, 950.0, padH, padW);
	Actors[nActors++] = (Actor*)m_Pads[m_numPads - 1];
	m_Pads[m_numPads++] = new Pad(this, 1500.0, 1000.0, padH, padW);
	Actors[nActors++] = (Actor*)m_Pads[m_numPads - 1];
	
	//create space debris
	for (int i = 0; i < MAXDEBRIS; i++)
	{
		m_Debris[m_numDebris++] = new Debris(this);
		Actors[nActors++] = (Actor*)m_Debris[m_numDebris - 1];
	}

	//random boost pickups
	srand(time(NULL));
	weaponBoostPad = rand() % m_numPads;
	movementBoostPad = rand() % m_numPads;
	m_Pads[weaponBoostPad]->weaponBoost = true;
	m_Pads[movementBoostPad]->movementBoost = true;


    // init variables
	m_iFrame = 0;

	m_worldMaxX = 1920;
	m_worldMaxY = 1440;
	m_screenWidth = 960;
	m_screenHeight = 720;

	m_xOffset = 0;
	m_yOffset = 0;
	m_margin = 280;

	m_gameState = INPLAY;

	m_oTime = m_fTime = clock();
	m_elapsedTime = 0.0;
	m_runTime = 0.0;

	thrustSoundTime = -2.f;
	alienEngineTime = 0.f;

	initializeSDL();
	done = 0;
	playingThrust = false;

	playerWeaponBoost = false;
	playerMovementBoost = false;

	wBoostStartTime = 0.f;
	mBoostStartTime = 0.f;

	wBoostDuration = 15.f;
	mBoostDuration = 15.f;

	return nActors;
}

int GameManager::gameOver()
{
	return done;
}

SDL_Renderer* GameManager::getRenderer()
{
	return renderer;
}

float GameManager::get_worldMaxY()
{
	return m_worldMaxY;
}

float GameManager::get_worldMaxX()
{
	return m_worldMaxX;
}

void GameManager::update_xOffset( )
{
	m_xOffset = m_screenWidth - m_margin - m_pRocket->X();

	if (m_xOffset > 0)
		m_xOffset = 0;

	if (m_xOffset < m_screenWidth - m_worldMaxX)
		m_xOffset = m_screenWidth - m_worldMaxX;
}

void GameManager::update_yOffset( )
{
	//m_yOffset += inc;
}


int GameManager::initializeSDL()
{	
	// Initialize SDL.
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return 1;

	if (TTF_Init() == -1)
	{
		printf("TTF init failed");
	}

	// Create the window where we will draw.
	window = SDL_CreateWindow("Rocket Lander - Tim Godwin",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		m_screenWidth, m_screenHeight,
		0);

	// We must call SDL_CreateRenderer in order for draw calls to affect this window.
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//initialize font
	font = TTF_OpenFont("Fonts/Terminator_Two.ttf", 24);
	redFontColor = { 255,0,0,255 };

	sText = TTF_RenderText_Solid(font, "Unidentified spacecraft approaching. Defend this airspace.", redFontColor);
	tText_UFO = SDL_CreateTextureFromSurface(renderer, sText);
	SDL_FreeSurface(sText);

	sText = TTF_RenderText_Solid(font, "All clear. Engage landing procedure.", redFontColor);
	tText_Clear = SDL_CreateTextureFromSurface(renderer, sText);
	SDL_FreeSurface(sText);

	sText = TTF_RenderText_Solid(font, "All clear. Engage landing procedure. Press 'R' to continue.", redFontColor);
	tText_Continue = SDL_CreateTextureFromSurface(renderer, sText);
	SDL_FreeSurface(sText);

	sText = TTF_RenderText_Solid(font, "Press 'R' to restart.", redFontColor);
	tText_Restart = SDL_CreateTextureFromSurface(renderer, sText);
	SDL_FreeSurface(sText);

	sText = TTF_RenderText_Solid(font, "Danger! Increase Altitude Immediately!", redFontColor);
	tText_Altitude = SDL_CreateTextureFromSurface(renderer, sText);
	SDL_FreeSurface(sText);

	sText = TTF_RenderText_Solid(font, "Warning. Unsafe landing vector. Adjust approach angle.", redFontColor);
	tText_Angle = SDL_CreateTextureFromSurface(renderer, sText);
	SDL_FreeSurface(sText);

	sText = TTF_RenderText_Solid(font, "Warning. Unsafe landing vector. Decrease flight speed.", redFontColor);
	tText_Speed = SDL_CreateTextureFromSurface(renderer, sText);
	SDL_FreeSurface(sText);
	

	//initialze sound
	SDL_AudioSpec fmt;
	//Set 16-bit stereo audio at 22Khz 
	fmt.freq = 44100;
	// fmt.freq = 22050;
	fmt.format = AUDIO_S16;
	fmt.channels = 2;
	fmt.samples = 512;        // A good value for games 
	fmt.callback = mixaudio;
	fmt.userdata = NULL;

	// Open the audio device and start playing sound! 
	if (SDL_OpenAudio(&fmt, &obtained) < 0) {
		fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_PauseAudio(0);


	keystates = SDL_GetKeyboardState( NULL );

	printf ("Starting...\n");

	return 1;
}

void GameManager::displayText_UFO()
{
	SDL_QueryTexture(tText_UFO, nullptr, nullptr, &rText.w, &rText.h);
	rText.x = 10;
	rText.y = 10;
	SDL_RenderCopy(renderer, tText_UFO, NULL, &rText);
}

void GameManager::displayText_Clear()
{
	SDL_QueryTexture(tText_Clear, nullptr, nullptr, &rText.w, &rText.h);
	rText.x = 10;
	rText.y = 10;
	SDL_RenderCopy(renderer, tText_Clear, NULL, &rText);
}

void GameManager::displayText_Continue()
{
	SDL_QueryTexture(tText_Continue, nullptr, nullptr, &rText.w, &rText.h);
	rText.x = 10;
	rText.y = 10;
	SDL_RenderCopy(renderer, tText_Continue, NULL, &rText);
}

void GameManager::displayText_Restart()
{
	SDL_QueryTexture(tText_Restart, nullptr, nullptr, &rText.w, &rText.h);
	rText.x = 340;
	rText.y = 350;
	SDL_RenderCopy(renderer, tText_Restart, NULL, &rText);
}

void GameManager::displayText_Altitude()
{
	SDL_QueryTexture(tText_Altitude, nullptr, nullptr, &rText.w, &rText.h);
	rText.x = 200;
	rText.y = 700;
	SDL_RenderCopy(renderer, tText_Altitude, NULL, &rText);
}

void GameManager::displayText_Angle()
{
	SDL_QueryTexture(tText_Angle, nullptr, nullptr, &rText.w, &rText.h);
	rText.x = 20;
	rText.y = 700;
	SDL_RenderCopy(renderer, tText_Angle, NULL, &rText);
}

void GameManager::displayText_Speed()
{
	SDL_QueryTexture(tText_Speed, nullptr, nullptr, &rText.w, &rText.h);
	rText.x = 20;
	rText.y = 700;
	SDL_RenderCopy(renderer, tText_Speed, NULL, &rText);
}


int GameManager::endGraphics()
{

	TTF_CloseFont(font);
	SDL_DestroyTexture(tText_UFO);
	TTF_Quit();

	//Quit SDL 
	SDL_Quit(); 

	return 1;
}

float GameManager::get_margin()
{
	return m_margin;
}

float GameManager::get_xOffset()
{
	return m_xOffset;
}

float GameManager::get_yOffset()
{
	return m_yOffset;
}

void GameManager::incTotalKills()
{
	totalKills++;
}

void GameManager::clearTotalKills()
{
	totalKills = 0;
}

void GameManager::setFlash()
{
	m_gameState = FLASH;
	clearTotalKills();
}

void GameManager::Update()
{  // get user input, update the game time, and do any global game update, i.e. check for a safe landing

	// get keyborad input
	UserInput();

	if (m_gameState == FLASH)
	{
		PlaySound(m_pRocket->getExplosionIndex());
		m_pRocket->setExplosionStartTime(m_runTime);
		m_gameState = EXPLODING;
	}

	if (m_gameState == EXPLODING && m_pRocket->getExplosionElapsedTime() > 1.8f)
	{
		m_gameState = NOTHING;
	}

	if (m_pAlien->myState == EXPLODING && m_pAlien->getExplosionElapsedTime() > 1.8f)
	{
		m_pAlien->setStateNothing();
	}

	// calc elapsed time, frame number
	m_currTime = clock();
	m_eTime = m_currTime - m_oTime;
	m_elapsedTime = (double) m_eTime / (double) CLOCKS_PER_SEC;
	m_runTime += m_elapsedTime;
	m_oTime = m_currTime;
	m_iFrame++;

	if ((m_iFrame % 300) == 0)
	{
		
		tpf = (float) ((m_currTime - m_fTime) / ((double) CLOCKS_PER_SEC * 60.0f));
		

		printf("Wave: %d\n", waveCount);
		printf("Kills: %d\n", waveKills);
		printf("Total Kills: %d\n", totalKills);

		if(waveComplete)
			printf("Wave complete\n");
		else
			printf("Wave not complete\n");

		if (m_pRocket->landed)
			printf("Rocket landed\n");
		else
			printf("Rocket not landed\n");

		printf("\n");
		

		
		m_fTime = m_currTime;
	}
	
	// add code for other screen boundaries
	if (m_pRocket->X() + m_pRocket->getW() > m_worldMaxX)
		m_pRocket->setX(m_worldMaxX - m_pRocket->getW());

	if (m_pRocket->X() < 0)
		m_pRocket->setX(0);
	

	//bottom boundary
	if (m_pRocket->Y() > m_worldMaxY)
	{
		if (m_gameState == INPLAY)
		{
			setFlash();
			//m_pRocket->setExplosionElapsedTime(0);
			//m_pRocket->reset();

			for (int i = 0; i < m_numPads; i++)
			{
				m_Pads[i]->setFuel(true);
			}
		}
	}

	//top boundary
	if (m_pRocket->Y() < 0)
		m_pRocket->setY(1);

	//scroll right
	if (m_pRocket->X() > m_screenWidth - m_xOffset - m_margin)
	{
		m_xOffset = m_screenWidth - m_margin - m_pRocket->X();
		
		if (m_xOffset < m_screenWidth - m_worldMaxX)
			m_xOffset = m_screenWidth - m_worldMaxX;
	}

	//scroll left
	else if (m_pRocket->X() < -m_xOffset + m_margin)
	{
		m_xOffset += -m_xOffset + m_margin - m_pRocket->X();

		if (m_xOffset > 0)
			m_xOffset = 0;
	}

	//scroll down
	if (m_pRocket->Y() > m_screenHeight - m_yOffset - m_margin)
	{
		m_yOffset =  m_screenHeight - m_margin - m_pRocket->Y();

		if (m_yOffset < m_screenHeight - m_worldMaxY)
			m_yOffset = m_screenHeight - m_worldMaxY;
	}

	//scroll up
	else if (m_pRocket->Y() <  -m_yOffset + m_margin)
	{
		m_yOffset += -m_yOffset + m_margin - m_pRocket->Y();

		if (m_yOffset > 0)
			m_yOffset = 0;
	}

	// for each landing pad, add code to check for landing
	for (int i = 0; i< m_numPads; i++)
	{ 
		if (padIntersect(m_Pads[i], m_pRocket))
		{
			if (m_pRocket->safeLanding() == 1)
			{
				if (m_Pads[i]->getFuel())
				{
					if (i == movementBoostPad)
					{
						playerMovementBoost = true;
						m_pRocket->refuel(1000);
						mBoostStartTime = m_runTime;
					}
					else if (i == weaponBoostPad)
					{
						playerWeaponBoost = true;
						wBoostStartTime = m_runTime;
					}

					m_pRocket->refuel(330);
					m_Pads[i]->setFuel(false);
				}
				m_pRocket->rest();
			}
			else if(m_gameState==INPLAY)
			{
				setFlash();
			}
		}
	}


	if (m_pAlien->inPlay == true && m_runTime - alienEngineTime > 2)
	{
		alienEngineTime = m_runTime;
		PlaySound(m_pAlien->getEngineIndex());
	}

	//spawn the Alien after 10 seconds if it hasnt already been killed
	if (m_runTime > 10 && !m_pAlien->destroyed)
	{		
		if(!m_pAlien->inPlay)
		{
			m_pAlien->inPlay = true;
			alienEngineTime = m_runTime;
			PlaySound(m_pAlien->getEngineIndex());
		}
	}


	//check for alien colliding with rocket
	if (m_gameState == INPLAY && 40*40 > (m_pRocket->X() - m_pAlien->X())*(m_pRocket->X() - m_pAlien->X()) + (m_pRocket->Y() - m_pAlien->Y())*(m_pRocket->Y() - m_pAlien->Y()))
	{
		setFlash();
	}

	//check for time to fire alien missile
	if (m_gameState == INPLAY && m_pAlien->inPlay && m_runTime - m_Missiles->alienLastFireTime > m_Missiles->alienMissileRate)
	{
		m_Missiles->launchMissile(false);
		PlaySound(m_Missiles->getAlienFireIndex());
	}

	//for each visible missile, check for collisions
	for (int i = 0; i < MAXMISSILES; i++)
	{
		if (m_gameState == INPLAY && m_Missiles->visibleMissiles[i])
		{
			//player missiles can hit alien
			if (m_Missiles->playerFriendlyMissiles[i])
			{
				if (30 * 30 >(m_pAlien->X() + m_pAlien->center.x - m_Missiles->pxMissiles[i])  *  (m_pAlien->X() + m_pAlien->center.x - m_Missiles->pxMissiles[i]) + (m_pAlien->Y() + m_pAlien->center.y - m_Missiles->pyMissiles[i]) * (m_pAlien->Y() + m_pAlien->center.y - m_Missiles->pyMissiles[i]))
				{
					PlaySound(m_Missiles->getMissileHitIndex());
					m_Missiles->explosionStartTime[i] = m_runTime;
					m_Missiles->explodingMissiles[i] = true;
					m_Missiles->visibleMissiles[i] = false;
					m_pAlien->healthPoints--;
				}
			}
			//alien missiles can hit player
			else
			{
				if(30 * 30 > (m_pRocket->X()+m_pRocket->center.x - m_Missiles->pxMissiles[i] )  *  (m_pRocket->X()+m_pRocket->center.x - m_Missiles->pxMissiles[i] ) + (m_pRocket->Y() + m_pRocket->center.y - m_Missiles->pyMissiles[i] ) * (m_pRocket->Y() + m_pRocket->center.y - m_Missiles->pyMissiles[i] ))
				{
					m_Missiles->visibleMissiles[i] = false;
					setFlash();
				}
			}
		}
	}

	//if the alien is dead, but do not double count while alien is dead and waiting for respawn
	if (m_pAlien->healthPoints <= 0  && m_runTime - lastKillTime > alienSpawnTime && !waveComplete)
	{
		waveKills++;
		lastKillTime = m_runTime;

		if (waveKills >= waveCount && !waveComplete)
		{
			waveCount++;
			waveComplete = true;
		}
	}

	//respawn the alien if its dead and the wave is not over, make sure this respawn happens before kill counted again
	if (!waveComplete && m_pAlien->healthPoints <= 0 && m_runTime - lastKillTime > alienSpawnTime  - 1)
	{
		m_pAlien->reset();
		m_Missiles->alienLastFireTime = m_runTime + 2.f;
	}


	//check for end of boosts
	if (playerWeaponBoost && m_runTime - wBoostStartTime > wBoostDuration)
		playerWeaponBoost = false;

	if (playerMovementBoost && m_runTime - mBoostStartTime > mBoostDuration)
		playerMovementBoost = false;

}//end of update


int GameManager::padIntersect(Pad* pPad, Rocket* pRocket)
{  // add code to check for rocket/pad intersection
	int result = 0;

	//if center X of Rocket is within X width of the pad
	if (pRocket->X() + pRocket->getW()/2 > pPad->X() && pRocket->X() + pRocket->getW()/2 < pPad->X() + pPad->getW())
	{
		//if bottom Y of Rocket is close within Y of pad 
		if (pRocket->Y() + pRocket->getH() - 5 > pPad->Y() - 2 && pRocket->Y() + pRocket->getH() - 5 < pPad->Y() + 2)
		{
			result = 1;
		}
	}

	return result;
}


void GameManager::Draw()
{ 	
	if(!waveComplete)
		displayText_UFO();
	else if(!m_pRocket->landed)
		displayText_Clear();
	else
		displayText_Continue();
	
	if (m_gameState != INPLAY)
		displayText_Restart();
	else if(m_pRocket->Y() > m_worldMaxY - 400)
		displayText_Altitude();
	else if(m_pRocket->angle > 28 || m_pRocket->angle < -28)
		displayText_Angle();
	else if (m_pRocket->getVy() > 90 || m_pRocket->getVx() > 100 || m_pRocket->getVx() < -100)
		displayText_Speed();
	
	SDL_RenderPresent(renderer);
}

void GameManager::UserInput()
{
	SDL_PumpEvents();

	if (keystates[SDL_SCANCODE_LSHIFT] || keystates[SDL_SCANCODE_RSHIFT])
	{
		if (m_Missiles->launchMissile(true))
		{
			PlaySound(m_Missiles->getMissileFireIndex());
		}
	}
	if (keystates[SDL_SCANCODE_X] )
	{
		setFlash();
	}
	if( keystates[ SDL_SCANCODE_LEFT ] || keystates[ SDL_SCANCODE_KP_4 ])
	{
		m_pRocket->DecAngle();
	}
	if( keystates[ SDL_SCANCODE_RIGHT ] || keystates[ SDL_SCANCODE_KP_6 ])
	{
		m_pRocket->IncAngle();
	}
	if( m_gameState == INPLAY && (keystates[ SDL_SCANCODE_UP ] || keystates[ SDL_SCANCODE_SPACE ]) && m_pRocket->getFuel() > 0)
	{
		m_pRocket->setThrusting(true);
		m_pRocket->Thrust();		

		//start rocket engine sound if 2 seconds since last playing it
		if(m_runTime - thrustSoundTime > 2.0f )
		{
			thrustSoundTime = m_runTime;
			PlaySound(m_pRocket->getEngineIndex());
			playingThrust = true;
		}
	}
	else
	{
		//if engine sound was playing, set sound time back 2 seconds so it can start again the next frame
		if (playingThrust)
		{
			playingThrust = false;
			thrustSoundTime = m_runTime - 2.f;
		}
		StopSound(m_pRocket->getEngineIndex());
		m_pRocket->setThrusting(false);
		m_pRocket->Drift();
	}
	if(( keystates[ SDL_SCANCODE_ESCAPE ] ) || ( keystates[ SDL_SCANCODE_Q ] ))
	{
		done = 1;
	}
	if (keystates[SDL_SCANCODE_R] && m_runTime - resetTime > 2.0f)
	{
		resetTime = m_runTime;

		//reset to wave 1 if R and wave not complete or rocket not landed
		if (!waveComplete || !m_pRocket->landed)
		{
			waveCount=1;
		}

		waveComplete = false;
		waveKills = 0;
		lastKillTime = 0.f;

		m_gameState = INPLAY;
		m_pRocket->reset();
		m_pAlien->reset();

		m_runTime = 0.f;
		resetTime = 0.f;
		thrustSoundTime = -2.f;
		alienEngineTime = 0.f;

		for (int i = 0; i < m_numPads; i++)
		{
			m_Pads[i]->reset();
		}

		for (int i = 0; i < MAXMISSILES; i++)
		{
			m_Missiles->reset(i);
		}

		for (int i = 0; i < MAXDEBRIS; i++)
		{
			m_Debris[i]->reset();
		}

		m_Missiles->resetMan();

		m_xOffset = 0;
		m_yOffset = 0;
		srand(time(NULL));
		weaponBoostPad = rand() % m_numPads;
		movementBoostPad = rand() % m_numPads;
		m_Pads[weaponBoostPad]->weaponBoost = true;
		m_Pads[movementBoostPad]->movementBoost = true;

		//add debris for # of kills
		for (int i = 0; i < totalKills; i++)
		{
			int rXpos = 100 + rand() % (int)m_worldMaxX;
			int rYpos = 100 + rand() % (int)m_worldMaxY;
			int rVx = rand() % 20;
			int rVy = rand() % 10;
			m_Debris[i]->spawnDebris(rXpos, rYpos, rVx, rVy);
		}


		wBoostStartTime = 0.f;
		mBoostStartTime = 0.f;
		playerWeaponBoost = false;
		playerMovementBoost = false;
	}

}


