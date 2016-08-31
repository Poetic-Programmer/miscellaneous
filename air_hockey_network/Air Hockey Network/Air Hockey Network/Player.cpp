#include "Player.h"

Player::Player()
{

}

Player::~Player()
{

}

void Player::Initialize(ID3D11Device* device)
{
	m_paddle.Initialize(device);
	m_position = Vector3(0, 9.25, -15);
	m_bounds = Circle(0, 0, 2);
}

void Player::Update(float deltaTime, Vector2 mousePos)
{
	m_position = Vector3((mousePos.x - 0.5f) * 15, 9.25, -mousePos.y * 20);

	m_velocity = (m_position - m_previousPosition) / deltaTime;

	if(m_position.x > 8.0f)
	{
		m_position.x = 8.0f;
	}
	if(m_position.x < -8.0f)
	{
		m_position.x = -8.0f;
	}
	if(m_position.z > 18.0f)
	{
		m_position.z = 18.0f;
	}
	if(m_position.z < -18.0f)
	{
		m_position.z = -18.0f;
	}
	m_paddle.SetPosition(m_position);
	m_bounds.SetCenterPosition(m_position.x, m_position.z);

	m_previousPosition = m_position;
}

void Player::Render(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{
	m_paddle.Render(dc, cam, lights);
}