/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/sound/sound_player/src/SoundPlayer.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * The SoundPlayer is responsible for playing sounds. It takes requests for changes
  * to the sounds being played and passes them on to its SoundPlayerThread.
  * 
  */

#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <string>
#include <map>

#include "SoundPlayerThread.h"

namespace TA_Base_Bus
{
	class ISound;
	class SoundPlayerThread;
	
	class SoundPlayer 
	{
	public:
		

		/**
		  * Constructor
		  */
		SoundPlayer();
		
		
		/**
		  * Destructor
		  */
		virtual ~SoundPlayer();
		
		
		/**
		  * playSound
		  * 
		  * Plays the sound represented by the given ISound object in the indicated position.
	  	  *
		  * Pre-condition: 0 <= position <= #categories
		  * Pre-condition: sound != NULL
		  *
          * @param position  Where in the sequence of categories of sounds, this sound should
          *                  be played.
		  * @param sound     The sound to play.
		  *
		  * @exception SoundPlayerException  Thrown if the sound cannot be played.
		  *
		  */
		void playSound(int position, ISound* sound);
		
		
		/**
		  * stopSound
		  * 
		  * Stops the playing of a sound in the given position.
          *
          * @param position  The position of the sound (in the sequence of sound categories)
          *                  that should be stopped.
		  *
		  */
		void stopSound(int position);

		
	private:

        /**
          * The SoundPlayerThread that periodically plays each category of sounds.
          */
		SoundPlayerThread m_soundPlayerThread;
	};

} // namespace TA_Base_Bus

#endif // SOUNDPLAYER_H
