/*
	file:	WaveManager.hpp
	Version:	1.0
	Date:	25th September 2012.
	Authors:	Stuart & Allan Milne.

	Namespace:	AllanMilne::Audio
	Exposes: WaveManager.
	Requires:	PCMWave.

	Description:
	Caches PCMWave objects representing raw audio data loaded from .wav files.

	When a .wav file load is requested first the manager checks if this file has already been loaded;
	* if already loaded it returns the cached PCMWave object, 
	* if not the data is loaded and cached for any further requests.

	Note this implementation is NOT thread safe.

	Exposed and implemented as a singleton pattern.
*/

#ifndef WAVEMANAGER_H
#define WAVEMANAGER_H

#include <string>
#include <map>
using std::string;
using std::map;

#include "Wave.h"

class WaveManager 
{
public:

	//--- Get the singleton instance - use lazy evaluation.
	static WaveManager& GetInstance (); 

	//--- Delete manager instance, will call instance destructor and hence all PCMWave destructors.
	static void Delete ();
	
	//--- Retrieve the PCMWave object for the named file; loads it into the cache if not already loaded.
	Wave *LoadWave (const std::wstring &aFileName);

	//--- delete named PCMWave object from cache; returns false if named file not loaded into cache.
	//--- NB will cause destructor of the relevant PCMWave object to be called and hence all associated sound sample data will be invalid.
	bool UnloadWave (const std::wstring &aFileName);

	//--- Returns true if named file is in the cache; false otherwise.
	bool IsLoaded (const std::wstring &aFileName);


private:
	// the singleton instance.
	static WaveManager *mInstance;

	// Constructor & destructor are hidden to implement singleton patern.
	WaveManager ();
	~WaveManager ();

	// the cache storing the loaded .wav file representations.
	map<std::wstring, Wave*> mWaveFileCache;

}; // end WaveManager class.

#endif