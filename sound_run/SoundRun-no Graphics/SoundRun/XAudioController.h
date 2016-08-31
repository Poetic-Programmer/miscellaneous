#ifndef XAUDIOCONTROLLER_H
#define XAUDIOCONTROLLER_H

#include <XAudio2.h>

class XAudioController
{
public:
	XAudioController();
	XAudioController(IXAudio2SourceVoice* voice, XAUDIO2_BUFFER buffer);
	~XAudioController();

	void Play(int flags);
	void Stop();
	void Pause();
	void UnPause();
	void Reset();

	void SetLooped(const bool loop);
	void SetVolume(const float volume);

	bool IsPlaying() const;

private:
	IXAudio2SourceVoice* m_sourceVoice;
	XAUDIO2_BUFFER       m_audioBuffer;
	bool m_isPaused;
};

#endif // XAudioController.h