/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/sound/sound_client/src/ISoundObserver.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Interface containing methods that are called when
  * an event is fired from the Sound Manager.
  *
  */

#ifndef ISOUNDOBSERVER_H
#define ISOUNDOBSERVER_H

#include <string>

namespace TA_Base_Bus
{

	class ISoundObserver
	{

	public:

		/** 
		  * handleCategorySilenced
		  *
		  * Called when a category of sounds has its silence state updated.
		  *
		  * @param category    The category of sounds that has been silenced or unsilenced.
		  * @param isSilenced  True if the category was silenced, and false if it was
		  *					   unsilenced.
		  *
		  */
		virtual void handleCategorySilenced(const std::string& category, bool isSilenced) = 0;


		/** 
		  * handleAllSilenced
		  *
		  * Called when all categories of sounds have been silenced or unsilenced.
		  *
		  * @param isSilenced  True if all sounds have been silenced, and false if they
		  *					   have been unsilenced.
		  */
		virtual void handleAllSilenced(bool isSilenced) = 0;

	};

} // namespace TA_Base_Bus

#endif // ISOUNDOBSERVER_H
