#ifndef GAME_H
#define GAME_H

#include "D3DApp.h"
#include "AirHockeyTable.h"
#include "Player.h"
#include "Puck.h"
#include "Vector2.h"
#include "LinearCurve.h"

class Camera;
struct DirectionalLight;

class Game
{
public:
	Game();
	~Game();

	void Initialize(ID3D11Device* device);
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);

	void SetPlayerInfo(Vector3 start, Vector3 end, float timeBetweenLastPacket);
	void SetPuckInfo(Vector3 position, Vector3 velocity);
private:
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
	float m_totalCurveTime;
	LinearCurve    m_curve;

	AirHockeyTable m_table;
	Player         m_player_1;
	Player         m_player_2;
	Puck           m_puck;
};

#endif // Game.h