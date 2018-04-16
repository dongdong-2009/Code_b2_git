/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADIProtocol.c $
  * @author:  Albertus Zakaria
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Public interface to CADI event processing
  */

#include <rpc/rpc.h>

#include "app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADIProtocol.h"
#include "cadcmd.h"

extern unsigned long gCadiProgramNumber;
extern int server_proc(u_long lProgram, u_long lVersion);

CADIEventCallback sEventCallback = 0;
void registerCadiEventCallback(CADIEventCallback cb)
{
    sEventCallback = cb;
	server_proc(gCadiProgramNumber, CADCMD__VERS);
}

void notifyEventObserver(enum CADIEventType eventType, void * cadiEvent)
{
    if (sEventCallback != 0)
    {
        sEventCallback(eventType, cadiEvent);
    }
}
