#include "Game.h"

#include <iostream>
using namespace std;


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

void Game::Update(float deltaTime)
{
	UpdatePlayer1(deltaTime);
	UpdatePlayer2(deltaTime);
	UpdatePuck(deltaTime);
}

void Game::SetPlayerInfo(Vector3 start, Vector3 end, float timeBetweenLastPacket)
{
	m_curve.Clear();
	m_curve.AddElement(start.x, start.y, start.z, 0);
	m_curve.AddElement(end.x, end.y, end.z, timeBetweenLastPacket);
	m_totalCurveTime = 0;
}

void Game::SetPuckInfo(Vector3 position, Vector3 velocity)
{
	m_puck.SetPosition(position);
	m_puck.SetVelocity(velocity);
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

void Game::UpdatePlayer1(float deltaTime)
{
	m_totalCurveTime += deltaTime;

	m_player_1.SetPosition(m_curve.Evaluate(m_totalCurveTime));
	m_player_1.Update(deltaTime);
}

void Game::UpdatePlayer2(float deltaTime)
{

}

void Game::UpdatePuck(float deltaTime)
{
	//cout << "Velocity < " << velocity.x << ", " << velocity.y << ", " << velocity.z << ">" << endl;
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