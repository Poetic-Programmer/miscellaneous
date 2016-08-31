#ifndef GAME_H
#define GAME_H

#include "D3DApp.h"
#include "AirHockeyTable.h"
#include "Player.h"
#include "Puck.h"
#include "Vector3.h"
#include "Vector2.h"
class Camera;

struct DirectionalLight;

class Game
{
public:
	Game();
	~Game();

	void Initialize(ID3D11Device* device);
	void Update(float deltaTime, Vector2 mousePos);
	void Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);

	Vector3 GetPlayerPosition();
	Vector3 GetPuckPosition();
	Vector3 GetPuckVelocity();
private:
	void CheckPuckPaddleCollision();

	void UpdatePlayer1(float deltaTime);
	void UpdatePlayer2(float deltaTime);
	void UpdatePuck(float deltaTime);

	void RenderBackground(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);
	void RenderTerrain(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);
	void RenderTable(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);
	void RenderPlayer1(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);
	void RenderPlayer2(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);
	void RenderPuck(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);

private:
	Vector2        m_mousePos;
	AirHockeyTable m_table;
	Player         m_player_1;
	Player         m_player_2;
	Puck           m_puck;
};

#endif // Game.h