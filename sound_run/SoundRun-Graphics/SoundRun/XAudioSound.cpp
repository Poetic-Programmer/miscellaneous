/*
	file:	XAudioSound.cpp
	Version:	1.1
	Date:	25th September 2012.
	Authors:	Stuart & Allan Milne.

	Namespace:	AllanMilne::Audio
	Exposes:	XAudioSound implementation.

	Description:
	This class models an XAudio2 sound that encapsulates a source voice and associated sound buffer
	It exposes basic playing behaviour is defined through the ISound interface
	and extends this with XAudio2 specific behaviour.
	this file contains the implementation of this class behaviour.

	this defines a playable audio sample with pausing and volume controls.
	A sound is ither stopped, paused or playing with the controls being defined for each of these states.

	the sound is represented by an XAudio2 source voice and XAudio2 buffer.
	The constructor requires both these components and is designed for use with the ISoundEngine::CreateSound() function.

*/


#include <XAudio2.h>

#include "XAudioSound.h"

//=== Behaviour extending the ISound interface.

//--- Constructor - provide an XAudio2 source voice and an XAudio2 buffer.
//--- Designed for use with the ISoundEngine::CreateSound() function.
XAudioSound::XAudioSound (IXAudio2SourceVoice *aVoice, XAUDIO2_BUFFER aBuffer)
: mIsPaused(false)
{
	mSourceVoice = aVoice;
	mXABuffer = aBuffer;
} // end XAudioSound constructor.

//--- Destructor function.
//--- Note we must not delete mSourceVoice here; a source voice is owned by the XAudio2 engine and will be deleted by it.
//--- Note also the order of destruction is important; XAudio2 destroys voices when the engine is destroyed, any calls to the voices AFTER this is an error, so any voice->DestroyVoice() should always be called before the engine is destroyed.
XAudioSound::~XAudioSound ()
{

} // end destructor function.

void XAudioSound::Clean()
{
	if (mSourceVoice != NULL)
	{
		mSourceVoice->Stop();
		mSourceVoice->FlushSourceBuffers();
		mSourceVoice->DestroyVoice();
	}
}

//--- Route this sound through the specified submix voice.
void XAudioSound::RouteToSubmixVoice (IXAudio2SubmixVoice* aSubmixVoice) 
{
	// Guard against null submix or source voices.
	if (aSubmixVoice==NULL || mSourceVoice==NULL) { return; }

	XAUDIO2_SEND_DESCRIPTOR sendDesc = { 0, aSubmixVoice };
	XAUDIO2_VOICE_SENDS sendList = { 1, &sendDesc };
	mSourceVoice->SetOutputVoices (&sendList);
} // end RouteToSubmixVoice function.


//=== Implementing the ISound interface.

//--- Play the sound with associated flags;
//--- always plays from the start of the sound sample, even if previously paused.
//--- No action if already playing.
void XAudioSound::Play (int aFlags)
{
	if (IsPlaying()) { return; }
	if (mIsPaused)
	{	// reset buffer.
		mSourceVoice->FlushSourceBuffers();
		mIsPaused = false;
	}
	mSourceVoice->SubmitSourceBuffer (&mXABuffer);
	mSourceVoice->Start (aFlags, XAUDIO2_COMMIT_NOW);
} // end Play method.

//--- Stop the sound plaing; play position is reset back to the start of the sound sample.
//--- If paused then sound position and pause state are reset.
//--- No action if already stopped.
void XAudioSound::Stop ()
{
	if (IsPlaying() || mIsPaused)
	{
		mSourceVoice->Stop();
		mSourceVoice->FlushSourceBuffers();
		mIsPaused = false;
	}
} // end Stop method.

//--- Pause the sound playing; play position is retained.
//--- No action if sound is stopped or already paused.
void XAudioSound::Pause ()
{
	if(IsPlaying())
	{
		mSourceVoice->Stop();
		mIsPaused = true;
	}
} // end Pause method.

//--- Resume playing from a paused state.
//--- No action if sound is stopped or playing.
void XAudioSound::Unpause ()
{
	if (mIsPaused)
	{
		mSourceVoice->Start(0);
		mIsPaused = false;
	}
} // end Unpause method.

//--- Toggle between play and pause state.
//--- No action if the sound is stopped.
void XAudioSound::TogglePause ()
{
	(mIsPaused) ? Unpause() : Pause();
} // end TogglePause method.

//--- Returns true if sound is actually playing; i.e. not stopped or paused.
bool XAudioSound::IsPlaying () const
{
	if (mIsPaused) { return false; }
	XAUDIO2_VOICE_STATE vState;
	mSourceVoice->GetState (&vState);
	return ( vState.BuffersQueued > 0 );
} // end IsPlaying method.

//--- Volume controls; actioned for all states.
//--- Use the XAudio2 interpretation of volume levels as amplitude multipliers;
//--- i.e. 1=native volume; 0=silence; <1=attenuation; >1=aplify; max=224; <0=phase inversion.

float XAudioSound::GetVolume () const
{
	float volume;
	mSourceVoice->GetVolume (&volume);
	return volume;
} // end GetVolume method.

//--- Does nothing if supplied value is out of XAudio2 allowable range.
void XAudioSound::SetVolume (const float aVolume)
{
	// Guard against to large or small values.
	if (aVolume<-XAUDIO2_MAX_VOLUME_LEVEL || aVolume>XAUDIO2_MAX_VOLUME_LEVEL) { return; }
	mSourceVoice->SetVolume (aVolume);
} // end SetVolume method.

//--- Does nothing if supplied value is out of XAudio2 allowable range.
//--- Parameter is a multiplier to apply to the XAudio2 volume multiplier;
//--- e.g. 3.0 will triple the volume multiplier; 0.5 will halve it.
void AdjustVolume (const float anIncrement);void XAudioSound::AdjustVolume (const float aMultiplier)
{
	float volume = GetVolume ();
	volume *= aMultiplier;
	SetVolume (volume);		// use the set method of this class to invoke the volume level guard.
} // end AdjustVolume method.

//--- Getter and setter to indicate if the sound should be played in looping mode.
//--- Setter is only actioned if the sound is in the stopped state.

void XAudioSound::SetLooped (const bool aLooped)
{
	//--- Guard against playing or paused.
	if (IsPlaying() || mIsPaused) { return; }
	if (aLooped) {
		mXABuffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	else {
		mXABuffer.LoopCount = 0;
	}
} // end SetLooped method.

bool XAudioSound::IsLooped() const
{
	return (mXABuffer.LoopCount == 0);
} // end GetLooped method.