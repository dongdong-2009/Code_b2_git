/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/sound/sound_player/src/DirectX8Sound.cpp $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * A DirectX8Sound is a sound that uses DirectX 8 to play
  */

#include "DirectX8Sound.h"
#include "DirectX8SoundManager.h"

#include "core/exceptions/src/SoundPlayerException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/Thread.h"

namespace TA_Base_Bus
{

	DirectX8Sound::DirectX8Sound(const std::string& wavFile)
		:m_wavFile(wavFile)
	{
		// TD 13705 Remove Assertion but throw SoundPlayerException so that any banner/app using
		// the sound manager will not die but log the exception
		// TA_ASSERT(DirectX8SoundManager::getInstance().isInitialised(), "DirectX8 not initialised!");
		if (FAILED( DirectX8SoundManager::getInstance().isInitialised() ) ||  DirectX8SoundManager::getInstance().isInitialised()==false) TA_THROW( TA_Base_Core::SoundPlayerException("DirectX8 not initialised!") );
		size_t nSize = wavFile.size() + 1;
		char* fileName = new char[ nSize ];
		memset(fileName,0,nSize);
		strcpy(fileName, wavFile.c_str());
		// Load the wave file into a DirectSound buffer
		if(FAILED(DirectX8SoundManager::getInstance().Create(&m_sound, fileName, DSBCAPS_GLOBALFOCUS, GUID_NULL)))
		{
			delete[] fileName;
			TA_THROW(TA_Base_Core::SoundPlayerException("Failed to load wave file into buffer"));
		}
		delete[] fileName;
	}
	
	
	DirectX8Sound::~DirectX8Sound()
	{
	}
	
	
	void DirectX8Sound::play()
	{
		if(FAILED(m_sound->Play( 0, 0 )))
		{
			TA_THROW(TA_Base_Core::SoundPlayerException("Failed to play sound: " + m_wavFile));
		}
		
		while (m_sound->IsSoundPlaying())
		{
			TA_Base_Core::Thread::sleep(100);
		}
	}
	
	
	void DirectX8Sound::stop()
	{
		m_sound->Stop();
		m_sound->Reset();
	}
	
	
	void DirectX8Sound::setFile(std::string filename)
	{
		TA_ASSERT(DirectX8SoundManager::getInstance().isInitialised(),
			"DirectX8 not initialised!");
		
		m_wavFile = filename;
		size_t nSize = filename.size() + 1;
		char* file = new char[ nSize ];
		memset(file,0,nSize);
		strcpy(file, filename.c_str());

		// Load the wave file into a DirectSound buffer
		if ( FAILED(DirectX8SoundManager::getInstance().Create(&m_sound, file, DSBCAPS_GLOBALFOCUS, GUID_NULL)) )
		{
			delete[] file;
			TA_THROW(TA_Base_Core::SoundPlayerException("Failed to load wave file into buffer"));
		}
		delete[] file;
	}

}
