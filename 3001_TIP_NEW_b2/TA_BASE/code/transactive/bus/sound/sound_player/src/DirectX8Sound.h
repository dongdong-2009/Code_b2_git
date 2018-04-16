/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/sound/sound_player/src/DirectX8Sound.h $
  * @author:  Daniel Hoey
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * A DirectX8Sound is a sound that uses DirectX 8 to play
  */

#ifndef DIRECTX8SOUND_H
#define DIRECTX8SOUND_H

#include "ISound.h"
#include "dsutil.h"
#include <string>

namespace TA_Base_Bus
{
		class DirectX8Sound : public ISound
		{
		public:
         /**
            * Constructor
            * 
            * Constructs a DirectX8Sound with the given wavFile. The 
            * DirectX8SoundManager must be initialised for construction to work.
            *
            * Pre-condition: DirectX8SoundManager::getInstance().isInitialised() == true
            *
            * @param wavFile - the path and name of the file to play
            *
            * @exception SoundPlayerException 
            *
            */
            DirectX8Sound(const std::string& wavFile);

		  /**
		    * Destructor
		    * 
		    */
		    virtual ~DirectX8Sound();

         /**
            * play
            * 
            * Plays the sound
            *
            * @exception SoundPlayerException - thrown when the sound cannot be played 
            */
            virtual void play();

          /**
            * stop
            * 
            * Stops the sound from playing
            *
            */
            virtual void stop();

          /**
            * setFile
            *
            * Sets the filename for this sound.
            *
            * @param filename  The filename containing the sound to be played.
            */
            virtual void setFile(std::string filename);

		
		private:
		    // Insert private methods and variables here
            CSound* m_sound;
            std::string m_wavFile;
		};
		
}
#endif // DIRECTX8SOUND_H
