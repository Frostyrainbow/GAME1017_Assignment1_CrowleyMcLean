#pragma once
#ifndef _INTERNALS_H_
#define _INTERNALS_H_

#include <iostream>
#include "SDL.h"
#define FPS 60
using namespace std;

bool g_running = false;
Uint32 g_start, g_end, g_delta, g_fps;
const Uint8* g_keystates;
SDL_Window* g_pWindow;
SDL_Renderer* g_pRenderer;

int Init(const char* title, int xPos, int yPos, int width, int height, int flags)
{
	cout << "Initializing engine..." << endl;
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) // If initialization is okay...
	{
		// Create the SDL window...
		g_pWindow = SDL_CreateWindow(title, xPos, yPos, width, height, flags);
		if (g_pWindow != nullptr)
		{
			// Create the SDL renderer... (back buffer)
			g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, NULL);
			if (g_pRenderer != nullptr)
			{
				// Initialize subsystems later...
			}
			else return false; // Renderer creation failed.
		}
		else return false; // Window creation failed.
	}
	else return false; // Initialization failed.
	g_fps = (Uint32)round(1 / (double)FPS) * 1000; // Converts FPS into milliseconds, e.g. 16.67
	g_keystates = SDL_GetKeyboardState(nullptr);
	cout << "Initialization successful!" << endl;
	g_running = true;
	return true;
}

void Clean()
{
	cout << "Cleaning engine..." << endl;
	SDL_DestroyRenderer(g_pRenderer);
	SDL_DestroyWindow(g_pWindow);
	SDL_Quit();
}

void Wake()
{
	g_start = SDL_GetTicks();
}

void HandleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			g_running = false;
			break;
		}
	}
}

void Update()
{

}

void Render()
{
	SDL_SetRenderDrawColor(g_pRenderer, 255, 0, 128, 255);
	SDL_RenderClear(g_pRenderer);
	// Any drawing here...

	SDL_RenderPresent(g_pRenderer); // Flip buffers - send data to window.
}

void Sleep()
{
	g_end = SDL_GetTicks();
	g_delta = g_end - g_start;
	if (g_delta < g_fps)
		SDL_Delay(g_fps - g_delta);
}

#endif