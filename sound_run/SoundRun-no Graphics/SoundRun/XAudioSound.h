/*
	file:	XASound.hpp
	Version:	1.1
	Date:	25th September 2012.
	Authors:	Stuart & Allan Milne.

	Namespace:	AllanMilne::Audio
	Exposes:	XASound.
	Requires:	ISound.

	Description:
	This class models an XAudio2 sound that encapsulates a source voice and associated sound buffer
	It exposes basic playing behaviour is defined through the ISound interface
	and extends this with XAudio2 specific behaviour.

	this defines a playable audio sample with pausing and volume controls.
	A sound is ither stopped, paused or playing with the controls being defined for each of these states.

	the sound is represented by an XAudio2 source voice and XAudio2 buffer.
	The constructor requires both these components and is designed for use with the ISoundEngine::CreateSound() function.

*/

#ifndef XAUDIOSOUND_H
#define XAUDIOSOUND_H

#include <XAudio2.h>

class XAudioSound
{
public:

	//=== Behaviour extending the ISound interface.

	//--- Constructor - provide an XAudio2 source voice and an XAudio2 buffer.
	//--- Designed for use with the ISoundEngine::CreateSound() function.
	XAudioSound (IXAudio2SourceVoice* aVoice, XAUDIO2_BUFFER aBuffer);

	//--- destructor.
	virtual ~XAudioSound ();

	//--- Access to the source voice to allow other methods to be called.
	inline IXAudio2SourceVoice* GetSourceVoice () const { return mSourceVoice; }

	//--- access to the XAudio2 buffer
	inline XAUDIO2_BUFFER& GetBuffer () { return mXABuffer; }

	//--- Route this sound through the specified submix voice.
	void RouteToSubmixVoice (IXAudio2SubmixVoice* aSubmixVoice);

	//=== Implementation of the ISound interface.

	//--- Play the sound with associated flags;
	//--- always plays from the start of the sound sample, even if previously paused.
	//--- No action if already playing.
	void Play (int aFlags);

	//--- Returns true if sound is actually playing; i.e. not stopped or paused.
	bool IsPlaying () const;

	//--- Stop the sound plaing; play position is reset back to the start of the sound sample.
	//--- If paused then sound position and pause state are reset.
	//--- No action if already stopped.
	void Stop ();

	//--- Pause the sound playing; play position is retained.
	//--- No action if sound is stopped or already paused.
	void Pause ();
	inline bool IsPaused () const { return mIsPaused; }

	//--- Resume playing from a paused state.
	//--- No action if sound is stopped or playing.
	void Unpause ();

	//--- Toggle between play and pause state.
	//--- No action if the sound is stopped.
	void TogglePause ();

	//--- Getter and setter to indicate if the sound should be played in looping mode.
	//--- Setter is only actioned if the sound is in the stopped state.
	void SetLooped (const bool aLooped);
	bool IsLooped () const;

	//--- Volume controls; actioned for all states.
	//--- Use the XAudio2 interpretation of volume levels as amplitude multipliers;
	//--- i.e. 1=native volume; 0=silence; <1=attenuation; >1=aplify; max=224; <0=phase inversion.
	float GetVolume () const;
	//--- Set and adjust methods do nothing if supplied value is out of XAudio2 allowable range.
	void SetVolume (const float aVolume);
	//--- Parameter is a multiplier to apply to the XAudio2 volume multiplier;
	//--- e.g. 3.0 will triple the volume multiplier; 0.5 will halve it.
	void AdjustVolume (const float aMultiplier);

	void Clean();

protected:		// Fields are protected to allow a subclass to extend this functionality.

	IXAudio2SourceVoice *mSourceVoice;
	XAUDIO2_BUFFER mXABuffer;
	bool				mIsPaused;

}; // end XASound class.

#endif