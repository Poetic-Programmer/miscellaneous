#ifndef PLAYER_H
#define PLAYER_H

#include "Vector3.h"
#include "Circle.h"
#include "Paddle.h"

class Player
{
public:
	Player();
	~Player();

	void Initialize(ID3D11Device* device);
	void Update(float deltaTime, Vector2 mousePos);
	void Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);

	Circle GetBounds() {return m_bounds;}

	Vector3 GetPosition() {return m_position;}
	Vector3 GetVelocity() {return m_velocity;}
private:
	Vector3 m_position;
	Vector3 m_previousPosition;
	Vector3 m_velocity;
	Circle m_bounds;
	Paddle m_paddle;
};

#endif // Player.h