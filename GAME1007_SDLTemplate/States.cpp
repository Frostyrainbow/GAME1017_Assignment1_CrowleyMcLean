#include "States.h"
#include "StateManager.h"
#include "Engine.h"
#include <iostream>
using namespace std;

void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}

void State::Resume() {}

// Begin TitleState
TitleState::TitleState() {}

void TitleState::Enter()
{
	cout << "Entering TitleState..." << endl;
}

void TitleState::Update()
{
	if (Engine::Instance().KeyDown(SDL_SCANCODE_N))
		STMA::ChangeState(new GameState());// Change to new GameState
}

void TitleState::Render()
{
	
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 0, 255, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	State::Render();
}

void TitleState::Exit()
{
	cout << "Exiting TitleState..." << endl;
}
// End TitleState

//Begin gamestate

GameState::GameState()
{
}

void GameState::Enter()
{
	
	Mix_PlayMusic(Engine::Instance().m_theme, -1); // 0-n for #number of loops, or -1 for infinite looping
	Mix_VolumeMusic(16); //0-128
	Mix_VolumeChunk(Engine::Instance().m_e_plane_fire, 8);
	Mix_VolumeChunk(Engine::Instance().m_p_tank_fire, 8);
	Mix_VolumeChunk(Engine::Instance().m_death, 13);
}

void GameState::Update()
{
	if (Engine::Instance().KeyDown(SDL_SCANCODE_X))
	{
		STMA::ChangeState(new TitleState());// Change to new TitleState
	}

	//Scroll the background
	Engine::Instance().m_bg1.GetDst()->y += Engine::Instance().m_speed/2;
	Engine::Instance().m_bg2.GetDst()->y += Engine::Instance().m_speed/2;
	//wrap backgrounds
	if (Engine::Instance().m_bg1.GetDst()->y >= HEIGHT)
	{
		//Bounce back to org pos
		Engine::Instance().m_bg1.GetDst()->y = 0;
		Engine::Instance().m_bg2.GetDst()->y = -HEIGHT;
	}

	if (Engine::Instance().KeyDown(SDL_SCANCODE_W) && Engine::Instance().m_player->GetDst()->y > HEIGHT/2)
	{
		Engine::Instance().m_player->GetDst()->y -= Engine::Instance().m_speed;
	}
	else if (Engine::Instance().KeyDown(SDL_SCANCODE_S) && Engine::Instance().m_player->GetDst()->y < HEIGHT - Engine::Instance().m_player->GetDst()->h)
	{
		Engine::Instance().m_player->GetDst()->y += Engine::Instance().m_speed;
	}
	else if (Engine::Instance().KeyDown(SDL_SCANCODE_A) && Engine::Instance().m_player->GetDst()->x > 0)
	{
		Engine::Instance().m_player->GetDst()->x -= Engine::Instance().m_speed;
	}
	else if (Engine::Instance().KeyDown(SDL_SCANCODE_D) && Engine::Instance().m_player->GetDst()->x < WIDTH - Engine::Instance().m_player->GetDst()->w)
	{
		Engine::Instance().m_player->GetDst()->x += Engine::Instance().m_speed;
	}

	for (int i = 0; i < Engine::Instance().m_bullets.size(); i++)
	{
		Engine::Instance().m_bullets[i]->Update(); // -> combines dereference and member access
	}

	//Check collision for bullet and enemys
	for (int i = 0; i < Engine::Instance().m_bullets.size(); i++)
	{
		for (int n = 0; n < Engine::Instance().m_enemys.size(); n++)
		{
			if (SDL_HasIntersection(Engine::Instance().m_bullets[i]->GetDst(), Engine::Instance().m_enemys[n]->GetDst()))
			{
				delete Engine::Instance().m_bullets[i];
				Engine::Instance().m_bullets[i] = nullptr;
				Engine::Instance().m_bullets.erase(Engine::Instance().m_bullets.begin() + i);
				Engine::Instance().m_bullets.shrink_to_fit();
				delete Engine::Instance().m_enemys[n];
				Engine::Instance().m_enemys[n] = nullptr;
				Engine::Instance().m_enemys.erase(Engine::Instance().m_enemys.begin() + n);
				Engine::Instance().m_enemys.shrink_to_fit();
				Mix_PlayChannel(-1, Engine::Instance().m_death, 0);
				cout << "Collision!" << endl;
				break;
			}
		}
		
	}

	//Check collision for bullet and rocks
	for (int i = 0; i < Engine::Instance().m_bullets.size(); i++)
	{
		for (int n = 0; n < Engine::Instance().m_rocks.size(); n++)
		{
			if (SDL_HasIntersection(Engine::Instance().m_bullets[i]->GetDst(), Engine::Instance().m_rocks[n]->GetDst()))
			{
				delete Engine::Instance().m_bullets[i];
				Engine::Instance().m_bullets[i] = nullptr;
				Engine::Instance().m_bullets.erase(Engine::Instance().m_bullets.begin() + i);
				Engine::Instance().m_bullets.shrink_to_fit();
				Mix_PlayChannel(-1, Engine::Instance().m_death, 0);
				cout << "Collision!" << endl;
				break;
			}
		}

	}
	
	
	//check for bullets going off screen
	for (int i = 0; i < Engine::Instance().m_bullets.size(); i++)
	{
		if (Engine::Instance().m_bullets[i]->GetDst()->y <= -Engine::Instance().m_bullets[i]->GetDst()->h) //off screen
		{
			cout << "Delete bullet" << endl;
			delete Engine::Instance().m_bullets[i];
			Engine::Instance().m_bullets[i] = nullptr;
			Engine::Instance().m_bullets.erase(Engine::Instance().m_bullets.begin() + i);
			Engine::Instance().m_bullets.shrink_to_fit();
			break;
		}
	}

	Engine::Instance().m_e_spawnTimer--;

	if (Engine::Instance().m_e_spawnTimer <= 0)
	{
		Engine::Instance().m_enemys.push_back(new Enemy({ rand() % 920 + 10 , -100 })); //min 10 max 920
		cout << "enemy spawned" << endl;
		Engine::Instance().m_e_spawnTimer = 70;
	}

	for (int i = 0; i < Engine::Instance().m_enemys.size(); i++)
	{
		Engine::Instance().m_enemys[i]->Update();
	}

	for (int i = 0; i < Engine::Instance().m_enemys.size(); i++)
	{
		if (Engine::Instance().m_enemys[i]->GetDst()->y >= 800)
		{
			delete Engine::Instance().m_enemys[i];
			Engine::Instance().m_enemys[i] = nullptr;
			Engine::Instance().m_enemys.erase(Engine::Instance().m_enemys.begin() + i);
			Engine::Instance().m_enemys.shrink_to_fit();
			cout << "Enemy deleted" << endl;
			break;
		}
	}



	for (int i = 0; i < Engine::Instance().m_enemys.size(); i++)
	{
		Engine::Instance().m_enemys[i]->m_missileSpawnTimer--;
		if (Engine::Instance().m_enemys[i]->m_missileSpawnTimer <= 0)
		{
			Engine::Instance().m_eBullets.push_back(new eBullet({ Engine::Instance().m_enemys[i]->GetDst()->x , Engine::Instance().m_enemys[i]->GetDst()->y + 30 }));
			Engine::Instance().m_eBullets.push_back(new eBullet({ Engine::Instance().m_enemys[i]->GetDst()->x + 80 , Engine::Instance().m_enemys[i]->GetDst()->y + 30 }));
			Mix_PlayChannel(-1, Engine::Instance().m_e_plane_fire, 0);
			cout << "Missile spawned" << endl;
			Engine::Instance().m_enemys[i]->m_missileSpawnTimer = 40;
		}
		
		
	}

	for (int i = 0; i < Engine::Instance().m_eBullets.size(); i++)
	{
		Engine::Instance().m_eBullets[i]->Update();
	}

	for (int i = 0; i < Engine::Instance().m_eBullets.size(); i++)
	{
		if (SDL_HasIntersection(Engine::Instance().m_eBullets[i]->GetDst(), Engine::Instance().m_player->GetDst()))
		{
			delete Engine::Instance().m_eBullets[i];
			Engine::Instance().m_eBullets[i] = nullptr;
			Engine::Instance().m_eBullets.erase(Engine::Instance().m_eBullets.begin() + i);
			Engine::Instance().m_eBullets.shrink_to_fit();
			delete Engine::Instance().m_player;
			Mix_PlayChannel(-1, Engine::Instance().m_death, 0);
			cout << "Collision!" << endl;
			break;
		}
	}

	for (int i = 0; i < Engine::Instance().m_eBullets.size(); i++)
	{
		if (Engine::Instance().m_eBullets[i]->GetDst()->y >= 800)
		{
			delete Engine::Instance().m_eBullets[i];
			Engine::Instance().m_eBullets[i] = nullptr;
			Engine::Instance().m_eBullets.erase(Engine::Instance().m_eBullets.begin() + i);
			Engine::Instance().m_eBullets.shrink_to_fit();
			cout << "Missile deleted" << endl;
			break;
		}
	}

	Engine::Instance().m_r_spawnTimer--;

	if (Engine::Instance().m_r_spawnTimer <= 0)
	{
		Engine::Instance().m_rocks.push_back(new Rock({ rand() % 920 + 10 , -100 })); //min 10 max 920
		cout << "rock spawned" << endl;
		Engine::Instance().m_r_spawnTimer = 120;
	}

	for (int i = 0; i < Engine::Instance().m_rocks.size(); i++)
	{
		Engine::Instance().m_rocks[i]->Update();
	}

	for (int i = 0; i < Engine::Instance().m_rocks.size(); i++)
	{
		if (Engine::Instance().m_rocks[i]->GetDst()->y >= 800)
		{
			delete Engine::Instance().m_rocks[i];
			Engine::Instance().m_rocks[i] = nullptr;
			Engine::Instance().m_rocks.erase(Engine::Instance().m_rocks.begin() + i);
			Engine::Instance().m_rocks.shrink_to_fit();
			cout << "Rock deleted" << endl;
			break;
		}
	}

	for (int i = 0; i < Engine::Instance().m_rocks.size(); i++)
	{
		if (SDL_HasIntersection(Engine::Instance().m_rocks[i]->GetDst(), Engine::Instance().m_player->GetDst()))
		{
			delete Engine::Instance().m_player;
			Mix_PlayChannel(-1, Engine::Instance().m_death, 0);
			cout << "Collision!" << endl;
			break;
		}
	}
		

}

void GameState::Render()
{
	//cout << "Rendering GameState..." << endl;
	
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 128, 255, 255);
	SDL_RenderClear(Engine::Instance().m_pRenderer);
	//Any drawing here...
	
	
	
	SDL_RenderCopy(Engine::Instance().m_pRenderer, Engine::Instance().m_pBGTexture, Engine::Instance().m_bg1.GetSrc(), Engine::Instance().m_bg1.GetDst());
	SDL_RenderCopy(Engine::Instance().m_pRenderer, Engine::Instance().m_pBGTexture, Engine::Instance().m_bg2.GetSrc(), Engine::Instance().m_bg2.GetDst());
	SDL_RenderCopyEx(Engine::Instance().m_pRenderer, Engine::Instance().m_pTexture, Engine::Instance().m_player->GetSrc(), Engine::Instance().m_player->GetDst(), 180.0, NULL, SDL_FLIP_NONE);
	for (int i = 0; i < Engine::Instance().m_bullets.size(); i++)
	{
		Engine::Instance().m_bullets[i]->Render(Engine::Instance().m_pRenderer, Engine::Instance().m_pShellTexture);
		//SDL_RenderCopy(m_pRenderer, m_pShellTexture, m_bullets[i]->GetDst(), m_bullets[i]->GetSrc());
	}
	for (int i = 0; i < Engine::Instance().m_rocks.size(); i++)
	{
		Engine::Instance().m_rocks[i]->Render(Engine::Instance().m_pRenderer, Engine::Instance().m_pRock);
	}
	for (int i = 0; i < Engine::Instance().m_enemys.size(); i++)
	{
		Engine::Instance().m_enemys[i]->Render(Engine::Instance().m_pRenderer, Engine::Instance().m_pEnemyTexture);
	}
	for (int i = 0; i < Engine::Instance().m_eBullets.size(); i++)
	{
		Engine::Instance().m_eBullets[i]->Render(Engine::Instance().m_pRenderer, Engine::Instance().m_pMissileTexture);
	}
	SDL_RenderPresent(Engine::Instance().m_pRenderer); // Flip buffers - send data to window.
	State::Render();
	
	
}

void GameState::Exit()
{
	cout << "Exiting GameState..." << endl;
}

void GameState::Resume()
{
	cout << "Resuming GameState..." << endl;
}
