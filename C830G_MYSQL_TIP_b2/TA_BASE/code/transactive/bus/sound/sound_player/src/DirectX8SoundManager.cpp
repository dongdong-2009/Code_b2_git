/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/sound/sound_player/src/DirectX8SoundManager.cpp $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * DirectX8SoundManager is a singleton wrapper for the CSoundManager class.
  */

#include <windows.h>
#include "DirectX8SoundManager.h"
#include "core/exceptions/src/SoundPlayerException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{

    DirectX8SoundManager::DirectX8SoundManager()
        :m_isInitialised(false)
    {
    }


    DirectX8SoundManager::~DirectX8SoundManager()
    {
    }


    DirectX8SoundManager& DirectX8SoundManager::getInstance()
    {
        static DirectX8SoundManager theSoundManager;
        return theSoundManager;
    }


    void DirectX8SoundManager::initialise(HWND window, bool isStereo, int frequency, int bitRate)
    {
        int numChannels = isStereo ? 2 : 1;
        if(FAILED(Initialize(window, DSSCL_NORMAL, numChannels, frequency, bitRate)))
        {
            TA_THROW(TA_Base_Core::SoundPlayerException("DirectX8SoundManager Initialisation failed"));
        }
        m_isInitialised = true;
    }


    void DirectX8SoundManager::initialise(bool isStereo, int frequency, int bitRate)
    {
        initialise(::GetDesktopWindow(), isStereo, frequency, bitRate);
    }


    void DirectX8SoundManager::initialise(HWND window)
    {
        initialise(window, true, 22050, 16);
    }


    void DirectX8SoundManager::initialise()
    {
        initialise(::GetDesktopWindow(), true, 22050, 16 );
    }


    bool DirectX8SoundManager::isInitialised()
    {
        return m_isInitialised;
    }
}
