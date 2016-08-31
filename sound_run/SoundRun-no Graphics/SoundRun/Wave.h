/*
	File:	Wave.hpp
	Version:	2.1
	Date:	21st September 2012.
	Authors:	Allan & stuart Milne.

	Namespace:	AllanMilne::Audio
	Exposes:	Wave.
	Requires:	IWave, WaveFormat.

	Description:
	Represents A sampled sound in PCM wave format.
	This defines the concrete class implementing the IWave interface.

	A Wave object includes a file name string, wave format information and the sample data values.
	An object can be created
	- by constructing it through loading a .wav file; or
	- by supplying the constructor with all attributes.
	Note that a private sample data buffer is always created when instantiating an object
	and the supplied sample data values copied to this buffer.

	Once instantiated only the file name and sample values can be modified;
	the format attributes and sample data buffer cannot be modified by the client.
	Accessor member functions are exposed to provide read-only access to the attributes;
	the wave format information is presented as a WaveFormat struct reference that then provides access to the wave format fields.

	The pointer and size of the buffer containing the sample value data is accessible but cannot be modified.
	Allowing client access to the buffer pointer allows the client to amend the data values of the buffer although the client cannot change either the address of the buffer or its size.

	Note that the class destructor frees the sample data buffer and so when a Wave object is de-allocated its buffer is no longer valid.

	A Wave object can also be saved to a file in .wav format.

	The object also exposes a status attribute that indicates if an error was detected when instantiating the object;
	possible values for this attribute are exposed as an enum in the interface.

*/


#ifndef __Wave_hpp_2_1
#define __Wave_hpp_2_1

#include <fstream>
#include <string>
using std::string;
using std::ifstream;
using std::ofstream;

#include "WaveFormat.h"

class Wave
{
public:
	enum Status {
		OK,
		Undefined,
		BufferAllocationError,
		FileOpenError,
		EndOfFile,
		RiffError,
		ChunkNotFound,
		RiffWriteError,
		NoFmtChunk,
		InvalidFmt,
		NotPcmFormat,
		InvalidFmtChunkSize,
		FmtWriteError,
		NoDataChunk,
		DataChunkError,
		DataWriteError
	};
	// Constructor with explicit definition of all attributes and sample value data.
	// Sample value data will be copied to a new buffer.
	Wave (const std::wstring &aFilename, unsigned short aChannels, unsigned short aBitDepth, unsigned long aSampleRate, size_t aSize, char *aData);

	// Constructor loading all attributes and sample value data from a specified .wav file.
	// The status attribute indicates if any errors occurred during file input.
	Wave (const std::wstring &aFilename);

	// Destructor.
	virtual ~Wave(); 

	// file name has a getter and setter.
	inline void SetFileName (std::wstring &aFilename) { mFileName = aFilename; }
	inline const std::wstring& GetFileName () const { return mFileName; }

	// Only getters are exposed for other attributes.
	inline const WaveFormat& GetWaveFormat () const { return mWaveFormat; }
	inline size_t GetDataSize () const { return mSize; }
	inline char* const GetWaveData () { return mWaveData; }
	inline Status GetStatus() const { return mStatus; }

	// computed from data size, number of channels and bit depth.
	unsigned int GetNumberOfSamples () const;
	unsigned int GetSampleLengthInMilliseconds () const;

	// Saves the Wave object to a file in .wav format; 
	// returns false if unsuccessful and sets the status field.
	// If file already exists then it is overwritten.
	bool SaveToFile ();				// saves to mFileName.
	bool SaveToFileAs (const std::wstring &aFilename);		// saves to the supplied file name; mFileName is changed.

	// return PCM wave status value as a string.
	std::wstring& GetStatusDescription () const;

private:
	std::wstring mFileName;		// file name of this sound sample.
	WaveFormat mWaveFormat;		// the wave format attributes.
	size_t mSize;			// number of bytes of sample data.
	char *mWaveData;		// Pointer to the sample sound data buffer.
	Status mStatus;	// indicates status of this object.

	//--- .wav file I/O helper functions.

	// read in the .wav chunks and initialise fields; returns whenever error detected.
	void ReadWaveFile (ifstream *anInfile);

	// find the chunk with tag = supplied ID (as  packed 4 character tag); 
	// file is positioned after the tag; if not found then file position is undefined.
	bool FindChunk (ifstream *anInFile, unsigned long aTag);

	// Write .wav file with the state of this object.
	void WriteWaveFile (ofstream *anOutfile);

}; // end Wave class.

#endif