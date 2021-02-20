#pragma once
#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#define FPS 60
#define WIDTH 1024
#define HEIGHT 768
using namespace std;


class Sprite 
{
protected:
	SDL_Rect m_src; //Source rect
	SDL_Rect m_dst; //Destination Rect
public:
	void SetRekts(SDL_Rect s, SDL_Rect d)
	{
		m_src = s;
		m_dst = d;
	}
	SDL_Rect* GetSrc()
	{
		return &m_src;
	}
	SDL_Rect* GetDst()
	{
		return &m_dst;
	}
};

class Button : public Sprite
{
public:
	Button(){}
	Button(SDL_Rect spawnLoc)
	{
		this->m_dst.x = spawnLoc.x; 
		this->m_dst.y = spawnLoc.y;
		this->m_dst.w = 189;
		this->m_dst.h = 58;
		m_src = { 0, 0, 189, 58 };
	}

	~Button()
	{
		
	}

	void Update()
	{
		
	}
	
	void SetLoc(SDL_Point loc)
	{
		m_dst.x = loc.x;
		m_dst.y = loc.y;
	}

	void Render(SDL_Renderer* rend, SDL_Texture* tex)
	{
		SDL_RenderCopyEx(rend, tex, &m_src, &m_dst, 0, NULL, SDL_FLIP_NONE);
	}

	bool GetPressed(Button button);

	void SetPressed(bool pressed)
	{
		m_Pressed = pressed;
	}
	
private:
	bool m_Pressed;
};

class Bullet : public Sprite
{
private:
	//SDL_Rect m_rect; //Single rect for destination
public:
	Bullet(SDL_Rect spawnLoc) //Non-default constructor
	{
		this->m_dst.x = spawnLoc.x; // this-> is optional
		this->m_dst.y = spawnLoc.y; // this-> is optional
		this->m_dst.w = 20;
		this->m_dst.h = 40;
		m_src = { 0, 0, 49, 296 };
		
		//m_rect = { spawnLoc.x, spawnLoc.y, 4, 4 }; short way
	}

	~Bullet() //Destructor
	{

	}

	void SetLoc(SDL_Point loc)
	{
		m_dst.x = loc.x;
		m_dst.y = loc.y;
	}

	void Update()
	{
		this->m_dst.y -= 5;
	}
	void Render(SDL_Renderer* rend, SDL_Texture* tex)
	{
		/*SDL_SetRenderDrawColor(rend, 0, 128, 255, 255);
		SDL_RenderFillRect(rend, &m_dst);*/
		SDL_RenderCopy(rend, tex, &m_src, &m_dst);
	}

	
	
};

class Enemy : public Sprite
{
public:
	int m_missileSpawnTimer = 40;
	Enemy(SDL_Rect spawnLoc)
	{
		this->m_dst.x = spawnLoc.x; // this-> is optional
		this->m_dst.y = spawnLoc.y; // this-> is optional
		this->m_dst.w = 100;
		this->m_dst.h = 100;
		this->m_src = { 0, 0, 835, 482 };
		
	}

	~Enemy()
	{

	}

	int getTimer()
	{
		
		return m_missileSpawnTimer;
	}

	void SetLoc(SDL_Point loc)
	{
		m_dst.x = loc.x;
		m_dst.y = loc.y;
	}

	void Update()
	{
		this->m_dst.y += 4;
	}
	void Render(SDL_Renderer* rend, SDL_Texture* tex)
	{
		SDL_RenderCopyEx(rend, tex, &m_src, &m_dst, 90.0, NULL, SDL_FLIP_NONE);
	}
	
};

class eBullet : public Sprite
{
private:
	//SDL_Rect m_rect; //Single rect for destination
public:
	eBullet(SDL_Rect spawnLoc) //Non-default constructor
	{
		this->m_dst.x = spawnLoc.x; // this-> is optional
		this->m_dst.y = spawnLoc.y; // this-> is optional
		this->m_dst.w = 20;
		this->m_dst.h = 40;
		m_src = { 0, 0, 415, 89 };

		//m_rect = { spawnLoc.x, spawnLoc.y, 4, 4 }; short way
	}

	~eBullet() //Destructor
	{

	}

	void SetLoc(SDL_Point loc)
	{
		m_dst.x = loc.x;
		m_dst.y = loc.y;
	}

	void Update()
	{
		this->m_dst.y += 5;
	}
	void Render(SDL_Renderer* rend, SDL_Texture* tex)
	{
		SDL_RenderCopyEx(rend, tex, &m_src, &m_dst, 270.0, NULL, SDL_FLIP_NONE);
	}

};

class Rock : public Sprite
{
public:
	
	Rock(SDL_Rect spawnLoc)
	{
		this->m_dst.x = spawnLoc.x; // this-> is optional
		this->m_dst.y = spawnLoc.y; // this-> is optional
		this->m_dst.w = 100;
		this->m_dst.h = 100;
		this->m_src = { 0, 0, 300, 300 };

	}

	~Rock()
	{

	}


	void SetLoc(SDL_Point loc)
	{
		m_dst.x = loc.x;
		m_dst.y = loc.y;
	}

	void Update()
	{
		this->m_dst.y += 2;
	}
	void Render(SDL_Renderer* rend, SDL_Texture* tex)
	{
		SDL_RenderCopy(rend, tex, &m_src, &m_dst);
	}

};


class Engine
{
public: // private properties.
	bool m_running = false;
	Uint32 m_start, m_end, m_delta, m_fps;
	const Uint8* m_keystates;
	SDL_Window* m_pWindow;
	SDL_Renderer* m_pRenderer;
	SDL_Texture* m_pTexture; //player texture
	SDL_Texture* m_pBGTexture; //background texture
	SDL_Texture* m_pShellTexture; //Player bullet texture
	SDL_Texture* m_pEnemyTexture; //Enemy texture
	SDL_Texture* m_pMissileTexture; //enemy bullets
	SDL_Texture* m_pRock; //Rock
	SDL_Texture* m_pResumeButtonTexture; // Resume button
	SDL_Texture* m_pMenuButtonTexture; //Main Menu button in End State
	SDL_Texture* m_pStartButtonTexture; //Start Button in Title
	SDL_Texture* m_pMenuBackgroundTexture; //Background for the title
	SDL_Texture* m_pGameOverBackgroundTexture; //Background for the title

	Sprite* m_player;
	Sprite m_bg1, m_bg2, m_pMenuBackground, m_pGameOverBackground;
	int m_speed = 5;
	int m_e_spawnTimer = 180;
	int m_r_spawnTimer = 260;
	
	int m_spawnLimit;
	vector<Bullet*> m_bullets;
	vector<eBullet*> m_eBullets;
	vector<Enemy*> m_enemys;
	vector<Rock*> m_rocks;
	Button m_pResumeButton;
	Button m_pMenuButton;
	Button m_pStartButton;
	//Sound effects
	Mix_Chunk* m_p_tank_fire;
	Mix_Chunk* m_e_plane_fire;
	Mix_Chunk* m_e_plane_spawn;
	Mix_Chunk* m_death;

	//Music tracks
	Mix_Music* m_theme;

	
	static SDL_Point m_mousePos;
	static Uint32 m_mouseCurr;
	static Uint32 m_mouseLast;
	SDL_Cursor* m_cursor;

private: // private method prototypes.
	Engine() {} //Prevents instantiantion outside class
	//~Engine() = delete; //Prevents destruction through pointer.
	int Init(const char* title, int xPos, int yPos, int width, int height, int flags);
	void Clean();
	void Wake();
	void HandleEvents();
	void Update();
	void Render();
	void Sleep();
	

public: // public method prototypes.
	int Run();
	bool KeyDown(SDL_Scancode c);
	static Engine& Instance(); //Static methoid for object access
	SDL_Renderer* GetRenderer() { return m_pRenderer; }

	static bool MousePressed(const int b);

	static SDL_Point& GetMousePos();
	
};

#endif

// Reminder: you can ONLY have declarations in headers, not logical code
