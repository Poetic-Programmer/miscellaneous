/*
	File:	WaveFormat.hpp
	Version:	2.0
	Date:	24th July 2012.
	Author:	Allan Milne.

	Namespace:	AllanMilne::Audio
	Exposes:	WaveFormat

	Description:
	Defines a struct representing the fields of an 'fmt' chunk of a .wav file.

	Uses the same member names as the WAVEFORMATEX struct defined in Microsoft DirectShow:
	http://msdn.microsoft.com/en-us/library/windows/desktop/dd390970(v=vs.85).aspx

	Uses C++ standard types rather than the Microsoft Windows WORD and DWORD types.
*/

#ifndef WaveFormat_H
#define WaveFormat_H

struct WaveFormat 
{
	unsigned short  wFormatTag;		// = 1 (WAVE_FORMAT_PCM) for .wav files.
	unsigned short  nChannels;		// usually 1 (mono) or 2 (stereo).
	unsigned long   nSamplesPerSec;		// in Hz; 44100 = CD quality.
	unsigned long   nAvgBytesPerSec;	// for PCM = nSamplesPerSec × nBlockAlign.
	unsigned short  nBlockAlign;		// in bytes; for PCM = nChannels × ((wBitsPerSample+7) \ 8)
	unsigned short  wBitsPerSample;		// often 8 or 16; in the .wav standard if not a multiple of 8 then each sample will still be aligned on byte boundary.
};


#endif