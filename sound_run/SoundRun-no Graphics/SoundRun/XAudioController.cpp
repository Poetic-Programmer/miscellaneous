#include "XAudioController.h"

XAudioController::XAudioController()
{
}

XAudioController::XAudioController(IXAudio2SourceVoice* voice, XAUDIO2_BUFFER buffer)
	: m_sourceVoice(voice),
	  m_audioBuffer(buffer)
{
}

XAudioController::~XAudioController()
{
	if(m_sourceVoice != NULL)
	{
		// Stop, flush, destroy
		m_sourceVoice->Stop();
		m_sourceVoice->FlushSourceBuffers();
		m_sourceVoice->DestroyVoice();
	}
}

/*
	void Play();
	void Stop();
	void Pause();
	void UnPause();
	void Reset();

	void SetLooped(const bool loop);
	void SetVolume(const float volume);
*/
void XAudioController::Play(int flags)
{
	// If already playing, exit method
	if(IsPlaying())
		return;

	if(m_isPaused)
	{
		m_sourceVoice->FlushSourceBuffers();
		m_isPaused = false;
	}

	m_sourceVoice->SubmitSourceBuffer(&m_audioBuffer);
	m_sourceVoice->Start(flags, XAUDIO2_COMMIT_NOW);
}

void XAudioController::Stop()
{
	if(IsPlaying() || m_isPaused)
	{
		m_sourceVoice->Stop();
		m_sourceVoice->FlushSourceBuffers();
		m_isPaused = false;
	}
}

void XAudioController::Pause()
{
	if(IsPlaying())
	{
		m_sourceVoice->Stop();
		m_isPaused = true;
	}
}

void XAudioController::UnPause()
{
	if(m_isPaused)
	{
		m_sourceVoice->Start(0);
		m_isPaused = false;
	}
}

void XAudioController::Reset()
{

}

void XAudioController::SetLooped(const bool loop)
{
	if(IsPlaying() || m_isPaused)
		return;

	if(loop)
		m_audioBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	else
		m_audioBuffer.LoopCount = 0;
}

void XAudioController::SetVolume(const float volume)
{
	// Check if volume is in range
	if(volume < -XAUDIO2_MAX_VOLUME_LEVEL || volume > XAUDIO2_MAX_VOLUME_LEVEL)
		return;
	m_sourceVoice->SetVolume(volume);
}

bool XAudioController::IsPlaying()const
{
	if(m_isPaused)
		return false;

	XAUDIO2_VOICE_STATE state;
	m_sourceVoice->GetState(&state);

	return(state.BuffersQueued > 0);
}