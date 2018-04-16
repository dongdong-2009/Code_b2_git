/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADIProtocol.h $
  * @author:  Albertus Zakaria
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Public interface to CADI event processing
  */


#include "app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADIEventType.h"

#ifndef __CADIEVENTADAPTER203424320499__
#define __CADIEVENTADAPTER203424320499__

#ifdef __cplusplus
extern "C" 
{
#endif
    
	typedef void (*CADIEventCallback)(enum CADIEventType, void *);


	/**
		Called by the client in order to register for events
	*/
	void registerCadiEventCallback(CADIEventCallback cb);

	/**
		Called from within the event receiver functions
	*/
	void notifyEventObserver(enum CADIEventType eventType, void * cadiEvent);

#ifdef __cplusplus
}
#endif


#endif
