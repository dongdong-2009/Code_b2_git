

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/signs/pids_manager/src/IPidMessageListener.h $
  * @author:  Anggiono
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Interface for class PidMessageSubscriber
  */

#ifndef IPIDMESSAGELISTENER_H
#define IPIDMESSAGELISTENER_H

#include "core/message/src/IGenericObserverListener.h"


// forward declaration
class PidMessage;

// Interface that is implemented by all classes wishing to receive updates to (adhoc and predefined) messages
class IPidMessageListener : public IGenericObserverListener 
{
public:
	virtual void onMessageChanged(PidMessage* message)=0;
    
};
#endif