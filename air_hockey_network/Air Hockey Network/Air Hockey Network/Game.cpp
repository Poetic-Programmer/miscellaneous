#include "Game.h"

Game::Game()
{

}

Game::~Game()
{

}

void Game::Initialize(ID3D11Device* device)
{
	m_table.Initialize(device);
	m_puck.Initialize(device);
	m_player_1.Initialize(device);
}

void Game::Update(float deltaTime, Vector2 mousePos)
{
	m_mousePos = mousePos;
	

	UpdatePlayer1(deltaTime);
	UpdatePlayer2(deltaTime);
	UpdatePuck(deltaTime);
	CheckPuckPaddleCollision();
}

void Game::Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{
	RenderBackground(dc, cam, lights);
	RenderTerrain(dc, cam, lights);
	RenderTable(dc, cam, lights);
	RenderPlayer1(dc, cam, lights);
	RenderPlayer2(dc, cam, lights);
	RenderPuck(dc, cam, lights);
}

void Game::CheckPuckPaddleCollision()
{
	    //float distance = c1.center.distSquared(c2.center);
        //float radiusSum = c1.radius + c2.radius;
        //return distance <= radiusSum * radiusSum;

	Vector2 temp(0, 0);
	temp = m_player_1.GetBounds().GetCenterPosition() - m_puck.GetBounds().GetCenterPosition();
	float distSquared = temp.x * temp.x + temp.y * temp.y;
	float radiusSum = m_player_1.GetBounds().GetRadius() * m_puck.GetBounds().GetRadius();

	if(distSquared <= radiusSum * radiusSum)
	{
		if(m_player_1.GetVelocity().length() == 0)
		{
			temp.Normalize();
			m_puck.SetPosition(m_player_1.GetBounds().GetCenterPosition() + (-temp * m_player_1.GetBounds().GetRadius()));
			m_puck.SetForce(Vector3(0, 0, 0));
		}
		else
		{
			m_puck.SetPosition(m_player_1.GetBounds().GetCenterPosition() + (-temp * m_player_1.GetBounds().GetRadius()));
			m_puck.AddForce(m_player_1.GetVelocity() / 10);
		}
	}
}

Vector3 Game::GetPlayerPosition()
{
	return m_player_1.GetPosition();
}

Vector3 Game::GetPuckPosition()
{
	return m_puck.GetPosition();
}

Vector3 Game::GetPuckVelocity()
{
	return m_puck.GetVelocity();
}

void Game::UpdatePlayer1(float deltaTime)
{
	m_player_1.Update(deltaTime, m_mousePos);
}

void Game::UpdatePlayer2(float deltaTime)
{

}

void Game::UpdatePuck(float deltaTime)
{
	m_puck.Update(deltaTime);
}

void Game::RenderBackground(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{

}

void Game::RenderTerrain(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{

}

void Game::RenderTable(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{
	m_table.Render(dc, cam, lights);
}

void Game::RenderPlayer1(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{
	m_player_1.Render(dc, cam, lights);
}

void Game::RenderPlayer2(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{

}

void Game::RenderPuck(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{
	m_puck.Render(dc, cam, lights);
}