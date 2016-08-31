/*
	file:	SoundEngine.hpp
	Version:	1.1
	Date:	25th September 2012.
	Authors:	Stuart and Allan Milne.

	Namespace:	AllanMilne::Audio
	Exposes:	SoundEngine
	Requires:	ISoundEngine, XASound, Wave.

	Description:
	this is the concrete SoundEngine class that encapsulates the core XAudio2 source voice functionality
	related to the XAudio2 processing engine and mastering voice.

	This is a concrete subclass of the ISoundEngine interface (that defined only behaviour);
	here we define both behaviour and outline implementation.

	this class exposes basic XAudio2 functionality for creating an engine with mastering voice
	together with a factory method for creating XASound objects (encapsulating buffer and voice).
	Clients can subclass SoundEngine exposing more sophisticated functionality as required;
	e.g. for managing XAudio 3-D and other client abstractions such as ambience.

*/

#ifndef SoundEngine_H
#define SoundEngine_H

#include <XAudio2.h>
#include <string>
using std::string;

#include "XAudioSound.h"
#include "Wave.h"

class SoundEngine
{
public:

	//--- constructor method will initialise the engine and mastering voice.
	SoundEngine();
    ~SoundEngine ();

	//--- Access to the XAudio2 engine and mastering voice; return NULL if errors.
	inline IXAudio2 *GetEngine () const { return mXAEngine; }
	inline IXAudio2MasteringVoice *GetMasterVoice () const { return mMasteringVoice; }

	//--- Accessors for other global/device attributes.
	inline int GetChannelCount () const { return mChannelCount; }
	inline XAUDIO2_DEVICE_DETAILS& GetDeviceDetails () { return mDetails; }

	//--- Overloaded factory methods to create an XASound object; return NULL if error.
	//--- Provide a .wav file name.
	XAudioSound *CreateSound (const std::wstring& aFilename);
	//--- Provide an encapsulated wave.
	XAudioSound *CreateSound (Wave *aWave);

     //--- Helper function to create the XASound object from MySound.wav with filter enabled.
     //XAudioSound* LoadXASound ()

protected:		// rather than private to allow clients to define an extended version.
	IXAudio2 *mXAEngine;
	IXAudio2MasteringVoice *mMasteringVoice;

	int mChannelCount;			// No. of channels on audio device
	XAUDIO2_DEVICE_DETAILS mDetails;	// Audio device information

}; // end SoundEngine interface.

#endif