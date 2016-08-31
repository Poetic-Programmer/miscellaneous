#include "Game.h"

Game::Game()
	: 
	m_soundEngine(),
	m_rainSFX(NULL),
	m_windSFX(NULL),
	m_gameOverSFX(NULL),
	m_gameEndingSound1(NULL),
	m_gameEndingSound2(NULL),
	m_music(NULL),
	m_trafficSFX(NULL),
	m_totalTime(0)
{
	m_gameOver = false;
}

Game::~Game()
{
	if(m_rainSFX != NULL)
	{
		m_rainSFX->Clean();
		SafeDelete(m_rainSFX);
	}
	if(m_windSFX != NULL)
	{
		m_windSFX->Clean();
		SafeDelete(m_windSFX);
	}
	if(m_gameOverSFX != NULL)
	{
		m_gameOverSFX->Clean();
		SafeDelete(m_gameOverSFX);
	}
	if(m_gameEndingSound1 != NULL)
	{
		m_gameEndingSound1->Clean();
		SafeDelete(m_gameEndingSound1);
	}
	if(m_gameEndingSound2 != NULL)
	{
		m_gameEndingSound2->Clean();
		SafeDelete(m_gameEndingSound2);
	}
	if(m_music != NULL)
	{
		m_music->Clean();
		SafeDelete(m_music);
	}
	if(m_trafficSFX != NULL)
	{
		SafeDelete(m_trafficSFX);
	}
}

bool Game::Initialize()
{	
	m_player.Initialize(m_soundEngine);
	m_monster.Initialize(m_soundEngine);

	if (m_soundEngine.GetEngine() == NULL || m_soundEngine.GetMasterVoice() == NULL) 
	{
		return false;
	}

	m_music = m_soundEngine.CreateSound(L"Music/RunMusic.wav");
	m_music->SetLooped(true);
	//m_music->Play(0);

	m_windSFX = m_soundEngine.CreateSound(L"Sound Effects/wind-howl-01-mono.wav");
	m_windSFX->SetLooped(true);
	m_windSFX->Play(0);

	m_rainSFX = m_soundEngine.CreateSound(L"Sound Effects/rain-03-mono.wav");
	m_rainSFX->SetLooped(true);
	m_rainSFX->Play(0);

	m_gameOverSFX = m_soundEngine.CreateSound(L"Sound Effects/Dark_Laugh-HopeinAwe-1491150192-mono.wav");
	m_gameOverSFX->SetLooped(false);

    m_gameEndingSound1 = m_soundEngine.CreateSound(L"Sound Effects/Stop.wav");
	m_gameEndingSound1->SetLooped(false);
	m_gameEndingSound1->SetVolume(5);

	m_gameEndingSound2 = m_soundEngine.CreateSound(L"Sound Effects/Dark_Laugh-HopeinAwe-1491150192-mono.wav");
	m_gameEndingSound2->SetLooped(false);

	m_trafficSFX = new X3DAudioSound(L"Sound Effects/street-traffic-2-mono.wav");
	m_trafficSFX->Initialize(m_soundEngine);
	m_trafficSFX->SetVolume(10); 

	m_trafficPath.AddElement(0, 0, 50, 0);
	m_trafficPath.AddElement(0, 0, 0, GameRunTime);

	//m_gameOverSFX->Play(0);
	m_player.SetActive(true);

	return true;
}

void Game::RestartGame()
{
	if(!m_music->IsPlaying())
		m_music->Play(0);

	if(!m_windSFX->IsPlaying())
		m_windSFX->Play(0);

	m_player.SetActive(true);
	m_monster.SetMonsterCatch(false);
	m_totalTime = 0;
	m_gameOver = false;
}

bool Game::IsGameOver()
{
	return(m_gameOver && !m_gameOverSFX->IsPlaying());
}

void Game::CheckCollisions()
{

}

void Game::CheckGameOver()
{
	if(m_player.GetBoundingCircle().CheckCircleCollision(m_monster.GetBoundingCircle()))
	{
		PlayGameOver();
		m_gameOver = true;
	}
}

void Game::CheckGameWon()
{
	if(m_totalTime >= GameRunTime)
	{
		PlayGameWon();
		m_gameOver = true;
	}
}

void Game::UpdateMonster(float deltaTime)
{

}

void Game::PlayGameOver()
{
	if(m_music->IsPlaying())
		m_music->Stop();

	if(!m_gameOverSFX->IsPlaying())
		m_gameOverSFX->Play(0);

	m_player.MoveToCentre();
	m_player.SetActive(false);
	m_monster.SetMonsterCatch(true);
}

void Game::PlayGameWon()
{
	if(m_music->IsPlaying())
		m_music->Stop();

	if(m_windSFX->IsPlaying())
		m_windSFX->Stop();

	m_player.MoveToCentre();
	m_player.SetActive(false);
	m_monster.SetActive(false);

	m_gameEndingSound1->Play(0);
}

void Game::UpdateMainMenu(float deltaTime)
{
	m_player.SetActive(false);
	m_monster.SetActive(false);

	if(m_music->IsPlaying())
		m_music->Pause();

}

void Game::UpdateGame(float deltaTime)
{
	m_totalTime += deltaTime;

	if(m_music->IsPaused())
		m_music->TogglePause();
	if(!m_trafficSFX->IsPlaying())
		m_trafficSFX->PlaySound();

	float z = (m_trafficPath.Evaluate(m_totalTime).z) * 0.2f;

	m_trafficSFX->SetEmittierPosition(0, 0, z, m_soundEngine);
	m_trafficSFX->UpdateSound(m_soundEngine);
	m_monster.Update(deltaTime, m_soundEngine);
	m_player.SetActive(true);
	m_monster.SetActive(true);
	m_player.Update(deltaTime, m_soundEngine);

	CheckGameOver();
	CheckGameWon();
	if (GetAsyncKeyState (VK_LEFT) & 0x0001) 
	{
		m_player.MoveLeft();
	}
	else if (GetAsyncKeyState (VK_RIGHT) & 0x0001) 
	{
		m_player.MoveRight();
	}
	else if(GetAsyncKeyState (VK_SPACE) & 0x0001)
	{
		m_player.MoveToCentre();
	}
	
}