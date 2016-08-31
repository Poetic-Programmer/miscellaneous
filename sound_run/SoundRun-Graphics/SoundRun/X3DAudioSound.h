#ifndef X3DAUDIOSOUND_H
#define X3DAUDIOSOUND_H

#include <Windows.h>
#include <X3DAudio.h>

#include "D3DUtility.h"
#include "XAudioController.h"
#include "SoundEngine.h"
#include "Wave.h"
#include "XAudioSound.h"
#include "WaveManager.h"

class SoundEngine;

class X3DAudioSound
{
public:
	X3DAudioSound(const std::wstring& filename);
	~X3DAudioSound();

	void Initialize(SoundEngine& engine);

	void UpdateSound(SoundEngine& engine);

	void PlaySound();

	void StopSound();

	void SetEmittierPosition(float x, float y, float z, SoundEngine& engine);

	void SetFrequency(float freq);

	void SetVolume(float volume);

	bool IsPlaying();
private:
	void InitX3DAudio (SoundEngine& engine);
	void InitListener(SoundEngine& engine);
	void InitEmitter(SoundEngine& engine);
	void InitDSP(SoundEngine& engine);
	void Apply3D(SoundEngine& engine);
	//--- Helper function to create the XASound object from MySound.wav with filter enabled.
    XAudioSound* LoadXASound (SoundEngine& engine);

private:
	std::wstring m_fileName;

	XAudioSound* m_waveSound;

	//--- X3DAudio elements.
	X3DAUDIO_HANDLE mX3DInstance;
	X3DAUDIO_LISTENER mListener;
	X3DAUDIO_EMITTER mEmitter;
	X3DAUDIO_DSP_SETTINGS mDSPSettings; 
};

#endif // X3DAudioSound.h