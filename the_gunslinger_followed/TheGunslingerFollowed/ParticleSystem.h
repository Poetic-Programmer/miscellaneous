#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "D3DUtil.h"
#include <string>
#include <vector>

class Camera;
class ParticleEffect;

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	// Time elapsed since the system was reset.
	float GetAge()const;

	void SetEyePos(const XMFLOAT3& eyePosW);
	void SetEmitPos(const XMFLOAT3& emitPosW);
	void SetEmitDir(const XMFLOAT3& emitDirW);

	void Initialize(ID3D11Device* device, ParticleEffect* fx, 
		ID3D11ShaderResourceView* texArraySRV, 
		ID3D11ShaderResourceView* randomTexSRV, 
		UINT maxParticles);

	void Reset();
	void Update(float dt, float gameTime);
	void Render(ID3D11DeviceContext* dc, const Camera& cam);

private:
	void BuildVB(ID3D11Device* device);

	ParticleSystem(const ParticleSystem& other);
	ParticleSystem& operator=(const ParticleSystem& other);

private:
	UINT m_maxParticles;
	bool m_firstRun;

	float m_gameTime;
	float m_timeStep;
	float m_age;

	XMFLOAT3 m_eyePosW;
	XMFLOAT3 m_emitPosW;
	XMFLOAT3 m_emitDirW;

	ParticleEffect* m_fx;

	ID3D11Buffer* m_initVB;	
	ID3D11Buffer* m_drawVB;
	ID3D11Buffer* m_streamOutVB;
 
	ID3D11ShaderResourceView* m_texArraySRV;
	ID3D11ShaderResourceView* m_randomTexSRV;
};

#endif