// Noninteractive, visual background image of the game world
#include "stdafx.h"
#include "SDL.h"
#include "Background.h"
#include "GameManager.h"


Background::Background(GameManager* pMan): Actor (pMan) {}

Background::~Background() {}

void Background::Load()
{
	sBackground = SDL_LoadBMP( "Sprites/my_background4.bmp" );
	tBackground = SDL_CreateTextureFromSurface(m_pGM->getRenderer(), sBackground);
	SDL_FreeSurface(sBackground);  // we got the texture now -> free surface

	rBackground.x = 0;
	rBackground.y = 0;
	rBackground.w = sBackground->w;
	rBackground.h = sBackground->h;
}

void Background::Draw()
{
	SDL_RenderCopy(m_pGM->getRenderer(), tBackground , NULL, &rBackground);
}

void Background::Update()
{	
	//change position based on screen offsets to enable background scrolling
	rBackground.x = m_pGM->get_xOffset();
	rBackground.y = m_pGM->get_yOffset();
}

	
