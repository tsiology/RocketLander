//Landing pads act as refueling stations and may also grant boosts
#include "stdafx.h"
#include "SDL.h"
#include "Pad.h"
#include "GameManager.h"

Pad::Pad(GameManager* pMan, float x, float y, float h, float w): Actor (pMan)
{
	rPad.x = (int)x;
	rPad.y = (int)y;
	rPad.h = (int)h;
	rPad.w = (int)w;

	drawPad.x = rPad.x;
	drawPad.y = rPad.y;
	drawPad.h = rPad.h;
	drawPad.w = rPad.w;

	fueledStation = true;

	weaponBoost = false;
	movementBoost = false;

}

void Pad::reset()
{
	fueledStation = true;
	weaponBoost = false;
	movementBoost = false;
}

float Pad::getH()
{
	return rPad.h;
}

float Pad::getW()
{
	return rPad.w;
}

bool Pad::getFuel()
{
	return fueledStation;
}

void Pad::setFuel(bool val)
{
	movementBoost = false;
	weaponBoost = false;
	fueledStation = val;
}

void Pad::Load()
{
	sPad = SDL_LoadBMP("Sprites/pad_art2.bmp");
	SDL_SetColorKey(sPad, SDL_TRUE, 0x0000ff00);
	tPad = SDL_CreateTextureFromSurface(m_pGM->getRenderer(), sPad);
	SDL_FreeSurface(sPad);  // we got the texture now -> free surface

	//weapon boosting pad
	sPad_wBoost = SDL_LoadBMP("Sprites/pad_art_wBoost2.bmp");
	SDL_SetColorKey(sPad_wBoost, SDL_TRUE, 0x0000ff00);
	tPad_wBoost = SDL_CreateTextureFromSurface(m_pGM->getRenderer(), sPad_wBoost);
	SDL_FreeSurface(sPad_wBoost);  // we got the texture now -> free surface

	//movement boosting pad
	sPad_mBoost = SDL_LoadBMP("Sprites/pad_art_mBoost2.bmp");
	SDL_SetColorKey(sPad_mBoost, SDL_TRUE, 0x0000ff00);
	tPad_mBoost = SDL_CreateTextureFromSurface(m_pGM->getRenderer(), sPad_mBoost);
	SDL_FreeSurface(sPad_mBoost);  // we got the texture now -> free surface
}


Pad::~Pad(){}

void Pad::Draw()
{
// add code to draw pad
	m_posX = (int)rPad.x;
	m_posY = (int)rPad.y;
	drawPad.x = rPad.x + m_pGM->get_xOffset();
	drawPad.y = rPad.y + m_pGM->get_yOffset();

	drawPadArt = drawPad;
	drawPadArt.y -= 8;
	drawPadArt.h = 34;

	if(fueledStation)
		SDL_SetRenderDrawColor(m_pGM->getRenderer(), 0x09, 0xC9, 0x09, 0xFF);
	else
		SDL_SetRenderDrawColor(m_pGM->getRenderer(), 0xC9, 0x09, 0x09, 0xFF);

	SDL_RenderFillRect(m_pGM->getRenderer(), &drawPad);

	if(weaponBoost)
		SDL_RenderCopyEx(m_pGM->getRenderer(), tPad_wBoost, NULL, &drawPadArt, 0, NULL, SDL_FLIP_NONE);
	else if(movementBoost)
		SDL_RenderCopyEx(m_pGM->getRenderer(), tPad_mBoost, NULL, &drawPadArt, 0, NULL, SDL_FLIP_NONE);
	else
		SDL_RenderCopyEx(m_pGM->getRenderer(), tPad, NULL, &drawPadArt, 0, NULL, SDL_FLIP_NONE);

       	
}

void Pad::Update()
{  // do nothing
 
}

	
