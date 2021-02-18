#include "Engine.h"


int Engine::Init(const char* title, int xPos, int yPos, int width, int height, int flags)
{
	srand(time(NULL));
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
				// Initialize subsystems
				if (IMG_Init(IMG_INIT_PNG || IMG_INIT_JPG) != 0)
				{
					m_pTexture = IMG_LoadTexture(m_pRenderer, "Img/tank.png");
					m_pBGTexture = IMG_LoadTexture(m_pRenderer, "Img/desert.jpg");
					m_pShellTexture = IMG_LoadTexture(m_pRenderer, "Img/tankround.png");
					m_pEnemyTexture = IMG_LoadTexture(m_pRenderer, "Img/plane.png");
					m_pMissileTexture = IMG_LoadTexture(m_pRenderer, "Img/missile.png");
					m_pRock = IMG_LoadTexture(m_pRenderer, "Img/rock.png");
					
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
	m_player = new Sprite;
	m_player->SetRekts ( {0, 0, 132, 254}, {512, 384, 100, 122} ); //First {} is the src rect and the second {} is the dest rect
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
	cout << "Initialization successful!" << endl;
	Mix_PlayMusic(m_theme, 1); // 0-n for #number of loops, or -1 for infinite looping
	Mix_VolumeMusic(16); //0-128
	Mix_VolumeChunk(m_e_plane_fire, 8);
	Mix_VolumeChunk(m_p_tank_fire, 8);
	Mix_VolumeChunk(m_death, 13);
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
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			m_running = false;
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
				case ' ':

					//Spawn bullet
					m_bullets.push_back(new Bullet({ m_player->GetDst()->x + 55, m_player->GetDst()->y - 50 }));
					
					
					Mix_PlayChannel(-1, m_p_tank_fire, 0); //-1 channel is first availiable
					break;
			}
			break;
		}
	}
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
	// Scroll the background
	m_bg1.GetDst()->y += m_speed/2;
	m_bg2.GetDst()->y += m_speed/2;
	//wrap backgrounds
	if (m_bg1.GetDst()->y >= HEIGHT)
	{
		//Bounce back to org pos
		m_bg1.GetDst()->y = 0;
		m_bg2.GetDst()->y = -HEIGHT;
	}

	if (KeyDown(SDL_SCANCODE_W) && m_player->GetDst()->y > HEIGHT/2)
	{
		m_player->GetDst()->y -= m_speed;
	}
	else if (KeyDown(SDL_SCANCODE_S) && m_player->GetDst()->y < HEIGHT - m_player->GetDst()->h)
	{
		m_player->GetDst()->y += m_speed;
	}
	else if (KeyDown(SDL_SCANCODE_A) && m_player->GetDst()->x > 0)
	{
		m_player->GetDst()->x -= m_speed;
	}
	else if (KeyDown(SDL_SCANCODE_D) && m_player->GetDst()->x < WIDTH - m_player->GetDst()->w)
	{
		m_player->GetDst()->x += m_speed;
	}

	for (int i = 0; i < m_bullets.size(); i++)
	{
		m_bullets[i]->Update(); // -> combines dereference and member access
	}

	// Check collision for bullet and enemys
	for (int i = 0; i < m_bullets.size(); i++)
	{
		for (int n = 0; n < m_enemys.size(); n++)
		{
			if (SDL_HasIntersection(m_bullets[i]->GetDst(), m_enemys[n]->GetDst()))
			{
				delete m_bullets[i];
				m_bullets[i] = nullptr;
				m_bullets.erase(m_bullets.begin() + i);
				m_bullets.shrink_to_fit();
				delete m_enemys[n];
				m_enemys[n] = nullptr;
				m_enemys.erase(m_enemys.begin() + n);
				m_enemys.shrink_to_fit();
				Mix_PlayChannel(-1, m_death, 0);
				cout << "Collision!" << endl;
				break;
			}
		}
		
	}

	// Check collision for bullet and rocks
	for (int i = 0; i < m_bullets.size(); i++)
	{
		for (int n = 0; n < m_rocks.size(); n++)
		{
			if (SDL_HasIntersection(m_bullets[i]->GetDst(), m_rocks[n]->GetDst()))
			{
				delete m_bullets[i];
				m_bullets[i] = nullptr;
				m_bullets.erase(m_bullets.begin() + i);
				m_bullets.shrink_to_fit();
				Mix_PlayChannel(-1, m_death, 0);
				cout << "Collision!" << endl;
				break;
			}
		}

	}
	
	
	//check for bullets going off screen
	for (int i = 0; i < m_bullets.size(); i++)
	{
		if (m_bullets[i]->GetDst()->y <= -m_bullets[i]->GetDst()->h) //off screen
		{
			//cout << "Delete bullet" << endl;
			delete m_bullets[i];
			m_bullets[i] = nullptr;
			m_bullets.erase(m_bullets.begin() + i);
			m_bullets.shrink_to_fit();
			break;
		}
	}

	m_e_spawnTimer--;

	if (m_e_spawnTimer <= 0)
	{
		m_enemys.push_back(new Enemy({ rand() % 920 + 10 , -100 })); //min 10 max 920
		cout << "enemy spawned" << endl;
		m_e_spawnTimer = 70;
	}

	for (int i = 0; i < m_enemys.size(); i++)
	{
		m_enemys[i]->Update();
	}

	for (int i = 0; i < m_enemys.size(); i++)
	{
		if (m_enemys[i]->GetDst()->y >= 800)
		{
			delete m_enemys[i];
			m_enemys[i] = nullptr;
			m_enemys.erase(m_enemys.begin() + i);
			m_enemys.shrink_to_fit();
			cout << "Enemy deleted" << endl;
			break;
		}
	}



	for (int i = 0; i < m_enemys.size(); i++)
	{
		m_enemys[i]->m_missileSpawnTimer--;
		if (m_enemys[i]->m_missileSpawnTimer <= 0)
		{
			m_eBullets.push_back(new eBullet({ m_enemys[i]->GetDst()->x , m_enemys[i]->GetDst()->y + 30 }));
			m_eBullets.push_back(new eBullet({ m_enemys[i]->GetDst()->x + 80 , m_enemys[i]->GetDst()->y + 30 }));
			Mix_PlayChannel(-1, m_e_plane_fire, 0);
			cout << "Missile spawned" << endl;
			m_enemys[i]->m_missileSpawnTimer = 40;
		}
		
		
	}

	for (int i = 0; i < m_eBullets.size(); i++)
	{
		m_eBullets[i]->Update();
	}

	for (int i = 0; i < m_eBullets.size(); i++)
	{
		if (SDL_HasIntersection(m_eBullets[i]->GetDst(), m_player->GetDst()))
		{
			delete m_eBullets[i];
			m_eBullets[i] = nullptr;
			m_eBullets.erase(m_eBullets.begin() + i);
			m_eBullets.shrink_to_fit();
			delete m_player;
			Mix_PlayChannel(-1, m_death, 0);
			cout << "Collision!" << endl;
			break;
		}
	}

	for (int i = 0; i < m_eBullets.size(); i++)
	{
		if (m_eBullets[i]->GetDst()->y >= 800)
		{
			delete m_eBullets[i];
			m_eBullets[i] = nullptr;
			m_eBullets.erase(m_eBullets.begin() + i);
			m_eBullets.shrink_to_fit();
			cout << "Missile deleted" << endl;
			break;
		}
	}

	m_r_spawnTimer--;

	if (m_r_spawnTimer <= 0)
	{
		m_rocks.push_back(new Rock({ rand() % 920 + 10 , -100 })); //min 10 max 920
		cout << "rock spawned" << endl;
		m_r_spawnTimer = 120;
	}

	for (int i = 0; i < m_rocks.size(); i++)
	{
		m_rocks[i]->Update();
	}

	for (int i = 0; i < m_rocks.size(); i++)
	{
		if (m_rocks[i]->GetDst()->y >= 800)
		{
			delete m_rocks[i];
			m_rocks[i] = nullptr;
			m_rocks.erase(m_rocks.begin() + i);
			m_rocks.shrink_to_fit();
			cout << "Rock deleted" << endl;
			break;
		}
	}

	for (int i = 0; i < m_rocks.size(); i++)
	{
		if (SDL_HasIntersection(m_rocks[i]->GetDst(), m_player->GetDst()))
		{
			delete m_player;
			Mix_PlayChannel(-1, m_death, 0);
			cout << "Collision!" << endl;
			break;
		}
	}

	
}

void Engine::Render()
{
	SDL_SetRenderDrawColor(m_pRenderer, 0, 128, 255, 255);
	SDL_RenderClear(m_pRenderer);
	// Any drawing here...
	
	
	
	SDL_RenderCopy(m_pRenderer, m_pBGTexture, m_bg1.GetSrc(), m_bg1.GetDst());
	SDL_RenderCopy(m_pRenderer, m_pBGTexture, m_bg2.GetSrc(), m_bg2.GetDst());
	SDL_RenderCopyEx(m_pRenderer, m_pTexture, m_player->GetSrc(), m_player->GetDst(), 180.0, NULL, SDL_FLIP_NONE);
	for (int i = 0; i < m_bullets.size(); i++)
	{
		m_bullets[i]->Render(m_pRenderer, m_pShellTexture);
		//SDL_RenderCopy(m_pRenderer, m_pShellTexture, m_bullets[i]->GetDst(), m_bullets[i]->GetSrc());
	}
	for (int i = 0; i < m_rocks.size(); i++)
	{
		m_rocks[i]->Render(m_pRenderer, m_pRock);
	}
	for (int i = 0; i < m_enemys.size(); i++)
	{
		m_enemys[i]->Render(m_pRenderer, m_pEnemyTexture);
	}
	for (int i = 0; i < m_eBullets.size(); i++)
	{
		m_eBullets[i]->Render(m_pRenderer, m_pMissileTexture);
	}
	SDL_RenderPresent(m_pRenderer); // Flip buffers - send data to window.
	
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


