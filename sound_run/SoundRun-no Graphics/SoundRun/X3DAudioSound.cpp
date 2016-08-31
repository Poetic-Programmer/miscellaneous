#include "X3DAudioSound.h"

X3DAudioSound::X3DAudioSound(const std::wstring& filename)
	: m_fileName(filename), m_waveSound(NULL)
{
	memset ((void*)&mX3DInstance, 0, sizeof (X3DAUDIO_HANDLE));
	memset ((void*)&mListener, 0, sizeof (X3DAUDIO_LISTENER));
	memset ((void*)&mEmitter, 0, sizeof (X3DAUDIO_EMITTER));
	memset ((void*)&mDSPSettings, 0, sizeof (X3DAUDIO_DSP_SETTINGS));
}

X3DAudioSound::~X3DAudioSound()
{
	SafeDelete(m_waveSound);
}

void X3DAudioSound::SetVolume(float volume)
{
	m_waveSound->SetVolume(volume);
}

void X3DAudioSound::Initialize(SoundEngine& engine)
{
	m_waveSound = LoadXASound(engine);
	
	m_waveSound->SetLooped(true);

	InitX3DAudio(engine);
	InitListener(engine);
	InitEmitter(engine);
	InitDSP(engine);

	
	//Apply3D(engine);		// raw .wav volume is not consistent with (0,0,0).
}

void X3DAudioSound::PlaySound()
{
	if(!m_waveSound->IsPlaying())
	{
		m_waveSound->Play(0);
	}
}

void X3DAudioSound::SetEmittierPosition(float x, float y, float z, SoundEngine& engine)
{
	mEmitter.Position.x = -x;
	mEmitter.Position.y = y;
	mEmitter.Position.z = z;

	
	Apply3D(engine);
}

bool X3DAudioSound::IsPlaying()
{
	if(m_waveSound->IsPlaying())
		return true;
	else
		return false;
}
void X3DAudioSound::UpdateSound(SoundEngine& engine)
{
	Apply3D(engine);
}

void X3DAudioSound::SetFrequency(float freq)
{
	m_waveSound->GetSourceVoice()->SetFrequencyRatio(freq, NULL);
}

void X3DAudioSound::InitX3DAudio (SoundEngine& engine)
{
	DWORD channelMask;
	XAUDIO2_DEVICE_DETAILS voiceDetails = engine.GetDeviceDetails();
	channelMask = voiceDetails.OutputFormat.dwChannelMask; 

	X3DAudioInitialize (channelMask, X3DAUDIO_SPEED_OF_SOUND, mX3DInstance);
}

void X3DAudioSound::InitListener (SoundEngine& engine)
{
	X3DAUDIO_VECTOR front = { 0.0f, 0.0f, 1.0f }; 
	X3DAUDIO_VECTOR top = { 0.0f, 1.0f, 0.0f }; 
	mListener.OrientFront = front;
	mListener.OrientTop = top;
	X3DAUDIO_VECTOR pos = { 0.0f, 0.0f, 0.0f }; 
	mListener.Position = pos;
}

void X3DAudioSound::StopSound()
{
	m_waveSound->Stop();
}

void X3DAudioSound::InitEmitter(SoundEngine& engine)
{
	XAUDIO2_VOICE_DETAILS details;
	m_waveSound->GetSourceVoice()->GetVoiceDetails(&details); 

	mEmitter.ChannelCount = details.InputChannels; 
	mEmitter.CurveDistanceScaler = 1.0f;
	X3DAUDIO_VECTOR pos = {0.0f, 0.0f, 0.0f }; 
	mEmitter.Position = pos;
	X3DAUDIO_VECTOR vel = { 0.0f, 10.0f, 0.0f }; 
	mEmitter.Velocity =vel;
}

void X3DAudioSound::InitDSP(SoundEngine& engine)
{
	mDSPSettings.SrcChannelCount = mEmitter.ChannelCount; 
	mDSPSettings.DstChannelCount = engine.GetChannelCount();
	mDSPSettings.pDelayTimes = NULL;
	mDSPSettings.pMatrixCoefficients = new FLOAT32 [mDSPSettings.SrcChannelCount * mDSPSettings.DstChannelCount]; 
}

void X3DAudioSound::Apply3D(SoundEngine& engine)
{
	X3DAudioCalculate (mX3DInstance, &mListener, &mEmitter, X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_LPF_DIRECT, &mDSPSettings );

	IXAudio2SourceVoice *source = m_waveSound->GetSourceVoice();
	source->SetOutputMatrix ( NULL, mDSPSettings.SrcChannelCount, mDSPSettings.DstChannelCount, mDSPSettings.pMatrixCoefficients); 
	source->SetFrequencyRatio (mDSPSettings.DopplerFactor); 
	XAUDIO2_FILTER_PARAMETERS FilterParameters = { LowPassFilter, 
		2.0f * sinf(X3DAUDIO_PI/6.0f * mDSPSettings.LPFDirectCoefficient), 
		1.0f }; 
	source->SetFilterParameters (&FilterParameters);
}

//--- Helper function to create the XASound object from MySound.wav with filter enabled.
XAudioSound* X3DAudioSound::LoadXASound (SoundEngine& engine)
{
	// First load the wave file and set up the format and buffer.
	WAVEFORMATEX wfmt;
	XAUDIO2_BUFFER xabuffer;
	Wave *wave = WaveManager::GetInstance().LoadWave (m_fileName);
	// Exit with NULL if file was not loaded correctly.
	if (wave->GetStatus() != Wave::OK) { return NULL; }
	memset ((void*)&wfmt, 0, sizeof (WAVEFORMATEX));
	memcpy_s ((void*)&wfmt, sizeof (WaveFormat), (void*)&(wave->GetWaveFormat()), sizeof (WaveFormat));
	memset ((void*)&xabuffer, 0, sizeof (XAUDIO2_BUFFER));
	xabuffer.AudioBytes = wave->GetDataSize ();
	xabuffer.pAudioData = (BYTE*)(wave->GetWaveData ());

	// Create the source voice.
	IXAudio2SourceVoice *source;
	HRESULT hr = engine.GetEngine()->CreateSourceVoice (&source, &wfmt, XAUDIO2_VOICE_USEFILTER);
	// exit with NULL if cannot create source voice.
	if (FAILED(hr)) { return NULL; }

	// return the XASound object.
	return (new XAudioSound (source, xabuffer));
} // end LoadXASound function.