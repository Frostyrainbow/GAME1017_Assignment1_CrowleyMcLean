#include "Engine.h"
#include "StateManager.h"
#include "States.h"

int Engine::Init(const char* title, int xPos, int yPos, int width, int height, int flags)
{
	cout << "Initializing engine..." << endl;
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) // If initialization is okay...
	{
		// Create the SDL window...
		m_pWindow = SDL_CreateWindow(title, xPos, yPos, width, height, flags);
		if (m_pWindow != nullptr)
		{
			// Create the SDL renderer...(back buffer)
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, NULL);
			if (m_pRenderer != nullptr)
			{
				 //Initialize subsystems
				if (IMG_Init(IMG_INIT_PNG || IMG_INIT_JPG) != 0)
				{
					m_pTexture = IMG_LoadTexture(m_pRenderer, "Img/tank.png");
					m_pBGTexture = IMG_LoadTexture(m_pRenderer, "Img/desert.jpg");
					m_pShellTexture = IMG_LoadTexture(m_pRenderer, "Img/tankround.png");
					m_pEnemyTexture = IMG_LoadTexture(m_pRenderer, "Img/plane.png");
					m_pMissileTexture = IMG_LoadTexture(m_pRenderer, "Img/missile.png");
					m_pRock = IMG_LoadTexture(m_pRenderer, "Img/rock.png");
					m_pResumeButtonTexture = IMG_LoadTexture(m_pRenderer, "Img/ResumeButton.png");
					m_pMenuButtonTexture = IMG_LoadTexture(m_pRenderer, "Img/MenuButton.png");
					m_pStartButtonTexture = IMG_LoadTexture(m_pRenderer, "Img/StartButton.png");
					m_pMenuBackgroundTexture = IMG_LoadTexture(m_pRenderer, "Img/Title.png");
					m_pGameOverBackgroundTexture = IMG_LoadTexture(m_pRenderer, "Img/Gameover.png");
					
				}
				else
				{
					return false; //Image init failed
				}
				if (Mix_Init(MIX_INIT_MP3) != 0)
				{
					//Configure mixer
					Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 2048);
					Mix_AllocateChannels(32);
					//Load sounds
					m_p_tank_fire = Mix_LoadWAV("Aud/p_tank_fire.wav");
					m_e_plane_fire = Mix_LoadWAV("Aud/e_plane_fire.wav");
					m_death = Mix_LoadWAV("Aud/boom.wav");
					m_theme = Mix_LoadMUS("Aud/CCR.mp3");
				}
				else
				{
					return false; //Mixer init failed
				}
			}
			else return false; // Renderer creation failed.
		}
		else return false; // Window creation failed.
	}
	else return false; // initalization failed.
	m_fps = (Uint32)round(1.0 / (double)FPS * 1000); // Converts FPS into milliseconds, e.g. 16.67
	m_keystates = SDL_GetKeyboardState(nullptr);
	//mouse init
	m_mouseCurr = SDL_GetMouseState(&m_mousePos.x, &m_mousePos.y);
	m_mouseLast = m_mouseCurr;
	
	m_player = new Sprite;
	m_player->SetRekts ( {0, 0, 132, 254}, {512, 384, 100, 122} ); 
	m_pResumeButton.SetRekts({ 0,0,189,58 }, { 420, 300, 189, 58 });
	m_pMenuButton.SetRekts({ 0,0,189,58 }, { 420, 200, 189, 58 });
	m_pStartButton.SetRekts({ 0,0,189,58 }, { 420, 400, 189, 58 });
	m_pMenuBackground.SetRekts({ 0, 0, WIDTH, HEIGHT }, { 0, 0, WIDTH, HEIGHT });
	m_pGameOverBackground.SetRekts({ 0, 0, WIDTH, HEIGHT }, { 0, 0, WIDTH, HEIGHT });
	m_bg1.SetRekts( {0, 0, WIDTH, HEIGHT}, {0, 0, WIDTH, HEIGHT} );
	m_bg2.SetRekts( {0, 0, WIDTH, HEIGHT}, {0, -HEIGHT, WIDTH, HEIGHT} );
	for (int i = 0; i < m_bullets.size(); i++)
	{
		m_bullets[i]->SetRekts( {0, 0, 49, 296}, {m_player->GetDst()->x, m_player->GetDst()->y, 16, 16} );
	}
	for (int i = 0; i < m_enemys.size(); i++)
	{
		m_enemys[i]->SetRekts({ 0, 0, 835, 482 }, { rand() % WIDTH + 0, -100, 30, 30 });
	}
	for (int i = 0; i < m_eBullets.size(); i++)
	{
		for (int n = 0; n < m_enemys.size(); n++)
		{
			m_eBullets[i]->SetRekts({ 0, 0, 415, 89 }, { m_enemys[n]->GetDst()->x, m_enemys[n]->GetDst()->y, 15, 15 });
		}
		
	}
	for (int i = 0; i < m_rocks.size(); i++)
	{
		m_rocks[i]->SetRekts({ 0, 0, 300, 300 }, { rand() % WIDTH + 0, -100, 40, 40 });
	}
	STMA::PushState(new TitleState());
	cout << "Initialization successful!" << endl;
	m_running = true;

	return true;
}

void Engine::Wake()
{
	m_start = SDL_GetTicks();
	
}

void Engine::HandleEvents()
{
	SDL_Event event;

	m_mouseLast = m_mouseCurr;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			m_running = false;
			break;
		
		}
	}

	m_mouseCurr = SDL_GetMouseState(&m_mousePos.x, &m_mousePos.y);
}

bool Engine::KeyDown(SDL_Scancode c)
{
	if (m_keystates != nullptr)
	{
		if (m_keystates[c] == 1)
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}




void Engine::Update()
{
	STMA::Update();
}

void Engine::Render()
{
	STMA::Render();
}

void Engine::Sleep()
{
	m_end = SDL_GetTicks();
	m_delta = m_end - m_start; // 1055 - 1050 = 5ms
	if (m_delta < m_fps)
		SDL_Delay(m_fps - m_delta);

}

int Engine::Run()
{
	if (m_running == true)
	{
		return 1;
	}
	// Start and run the "engine"
	if (Init("GAME1007 M1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, NULL) == false)
	{
		return 2;
	}
	// We passed our initial checks, start the loop!
	while (m_running == true)
	{
		Wake();
		HandleEvents(); // Input
		Update();       // Processing
		Render();       // Output
		if (m_running == true)
			Sleep();
	}
	Clean();
	return 0;
}


//Engine* Engine::Instance()
//{
//	static Engine* instance = new Engine(); //Magic statics creating the object
//	return instance;
//} pointer way

Engine& Engine::Instance()
{
	static Engine instance; //Magic statics creating the object
	return instance;
}



bool Engine::MousePressed(const int b)
{
	return ((m_mouseCurr & SDL_BUTTON(b)) > (m_mouseLast & SDL_BUTTON(b)));
}


SDL_Point& Engine::GetMousePos()
{
	return m_mousePos;
}

bool Button::GetPressed(Button button)
{
	const auto mousePos = Engine::GetMousePos();
	if (SDL_PointInRect(&mousePos, button.GetDst()) && Engine::MousePressed(1))
	{
		m_Pressed = true;
	}
	return m_Pressed;
}



void Engine::Clean()
{
	cout << "Cleaning engine..." << endl;
	for (int i = 0; i < m_bullets.size(); i++)
	{
		delete m_bullets[i];
		m_bullets[i] = nullptr;
	}
	m_bullets.clear();
	m_bullets.shrink_to_fit();
	for (int i = 0; i < m_enemys.size(); i++)
	{
		delete m_enemys[i];
		m_enemys[i] = nullptr;
	}
	m_enemys.clear();
	m_enemys.shrink_to_fit();
	for (int i = 0; i < m_eBullets.size(); i++)
	{
		delete m_eBullets[i];
		m_eBullets[i] = nullptr;
	}
	m_eBullets.clear();
	m_eBullets.shrink_to_fit();
	for (int i = 0; i < m_rocks.size(); i++)
	{
		delete m_rocks[i];
		m_rocks[i] = nullptr;
	}
	m_rocks.clear();
	m_rocks.shrink_to_fit();
	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	SDL_DestroyTexture(m_pTexture);
	SDL_DestroyTexture(m_pBGTexture);
	SDL_DestroyTexture(m_pShellTexture);
	SDL_DestroyTexture(m_pEnemyTexture);
	SDL_DestroyTexture(m_pMissileTexture);
	SDL_DestroyTexture(m_pRock);
	Mix_FreeChunk(m_p_tank_fire);
	Mix_FreeChunk(m_e_plane_fire);
	Mix_FreeChunk(m_death);
	Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}


Uint32 Engine::m_mouseCurr;
Uint32 Engine::m_mouseLast;
SDL_Point Engine::m_mousePos;



