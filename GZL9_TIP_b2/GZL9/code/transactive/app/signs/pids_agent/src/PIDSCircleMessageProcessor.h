/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/PIDSClearRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#ifndef PIDS_CIRCLE_MESSAGE_PROCESSOR_H_INCLUDED
#define PIDS_CIRCLE_MESSAGE_PROCESSOR_H_INCLUDED
#include "PIDSManager.h"
#include "PIDSAdHocDisplayRequest.h"
#include "PIDSClearRequest.h"
#include "ace/Timer_Queue_Adapters.h"
#include "ace/Event_Handler.h"
#include "ace/Timer_Heap.h"
#include <vector>

namespace TA_IRS_App
{
    typedef ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap> ActiveTimer;
    typedef TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime MessageTime;    

    class PIDSCircleMessageProcessor:public ACE_Event_Handler
    {
    public:
        //Time out handle method
        virtual int handle_timeout( const ACE_Time_Value &tv, const void *arg );

        //Circle request structure
        struct PIDSAdhocData 
        {
            PIDSAdHocDisplayRequest* m_reQuest;
            long m_startTime;
            long m_endTime;
		    bool m_isSend;
            ~PIDSAdhocData(){delete m_reQuest;}
        };
        static PIDSCircleMessageProcessor *getInstance();
        ~PIDSCircleMessageProcessor();

        //Put a circle request object to m_adHoc
        void addCirleMessage(PIDSAdhocData* adhocData);        
        void addCirleMessage(PIDSAdHocDisplayRequest* reQuest, MessageTime timeStart, MessageTime timeEnd);
        
        //Put a clear request to m_clearRe;
        void addClearMessage(PIDSClearRequest* clRequest);
        void addClearMessage(
            TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList destinationList,
            bool isTrain);
    private:
        PIDSCircleMessageProcessor();

        //Timer
        ActiveTimer m_timerMgr;     

		//The one and only one class instance
        static PIDSCircleMessageProcessor *m_theClass;

        //Interval
        static ACE_Time_Value m_interval;

        //Vector saved circle request
        std::vector<PIDSAdhocData*> m_adHoc;

        //Vector saved circle clear request
        std::vector<PIDSClearRequest*> m_clearRe;

        //Timer id,identify a timer
        long m_timerId;
        
        //Guard data access
        static TA_Base_Core::ReEntrantThreadLockable m_handleAccessLock;
    };
}
#endif