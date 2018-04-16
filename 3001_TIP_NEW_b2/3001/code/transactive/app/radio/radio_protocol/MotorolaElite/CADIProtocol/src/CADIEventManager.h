

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventManager.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */

#ifndef __CADI_EventManager_H
#define __CADI_EventManager_H


#include "ace/Task.h"
#include "ace/Singleton.h"
#include "ace/Activation_Queue.h"

#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventSubject.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADIEventType.h"


namespace TA_IRS_App
{
    class CADIEventManager : public ACE_Task_Base, public CADIEventSubject
    {
    public:
        CADIEventManager();
        virtual ~CADIEventManager();

        int shutdown();
        virtual int svc();
        int perform(ACE_Method_Request *req);

        static void eventCallback(CADIEventType et, void* arg);
        
        void startEventListener();
        void stopEventListener();

    private:
        bool m_shutdown;
        ACE_Activation_Queue m_queue;

        // disable methods
        CADIEventManager(const CADIEventManager&);
        void operator=(const CADIEventManager&);

    };

    typedef ACE_Singleton<CADIEventManager, ACE_Null_Mutex> cadiEventManager;
};

#endif
