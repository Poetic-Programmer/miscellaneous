/*
	file:	WaveManager.cpp
	Version:	1.0
	Date:	25th September 2012.
	Authors:	Stuart & Allan Milne.

	Namespace:	AllanMilne::Audio
	Exposes: WaveManager.
	Requires:	PCMWave.

	Description:
	Caches PCMWave objects representing raw audio data loaded from .wav files.
	this is the implementation file - see WaveManager.hpp for details.
*/

#include <string>
#include <map>
using std::string;
using std::map;

#include "WaveManager.h"
#include "Wave.h"

//--- Initialise the static instance field.
WaveManager *WaveManager::mInstance = NULL;

//--- Get the singleton instance - use lazy evaluation.
WaveManager& WaveManager::GetInstance () 
{
	// Instance already created so return it.
	if (mInstance) { return *mInstance; }
	// First time called so instantiate the object.
	mInstance = new WaveManager ();
	return *mInstance;
} // end static GetInstance function.

//--- Delete manager instance, will call instance destructor and hence all PCMWave destructors.
void WaveManager::Delete ()
{
	delete mInstance;
	mInstance = NULL;
} // end static Delete function.

//--- Returns true if named file is in the cache; false otherwise.
bool WaveManager::IsLoaded (const std::wstring &aFileName)
{
	if (mWaveFileCache.count(aFileName) == 0) {
		return false;
	} else {
		return true;
	}
} // end IsLoaded function.

//--- Retrieve the PCMWave object for the named file; loads it into the cache if not already loaded.
Wave *WaveManager::LoadWave (const std::wstring &aFileName)
{
	// If this file has not been loaded then add it to the cache.
	if (mWaveFileCache.count(aFileName) == 0) {
		Wave *wave = new Wave (aFileName);
		mWaveFileCache.insert (std::pair<std::wstring, Wave*> (aFileName, wave) );
	}
	return mWaveFileCache[aFileName];
} // end LoadWave function.

//--- delete named PCMWave object from cache; returns false if named file not loaded into cache.
//--- NB will cause destructor of the relevant PCMWave object to be called and hence all associated sound sample data will be invalid.
bool WaveManager::UnloadWave (const std::wstring &aFileName)
{
	// If this file has not been loaded we can return false now.
	if (mWaveFileCache.count(aFileName) == 0) { return false; }
	// delete the PCMWave object.
	delete mWaveFileCache[aFileName];
	// Remove entry from the cache.
	mWaveFileCache.erase (aFileName);
	return true;
} // end UnloadWave function.

// Constructor & destructor are hidden to implement singleton patern.
WaveManager::WaveManager ()
{
} // end WaveManager constructor.

WaveManager::	~WaveManager ()
{
	//mWaveFileCache.
	// Delete all the PCMWave objects in the cache.
	for (map<std::wstring, Wave*>::iterator wave=mWaveFileCache.begin(); wave!=mWaveFileCache.end(); ++wave) {
		delete wave->second;		// The PCMWave object pointer of the map <key,value> pair.
	}
} // end WaveManager destructor.