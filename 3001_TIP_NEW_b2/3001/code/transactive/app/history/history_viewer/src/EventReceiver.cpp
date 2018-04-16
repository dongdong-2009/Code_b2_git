/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/history_viewer/src/EventReceiver.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// EventReceiver.cpp: implementation of the CEventReceiver class.

//

//////////////////////////////////////////////////////////////////////



#include "stdafx.h"

#include "historyviewer.h"

#include "EventReceiver.h"

//#include "CommonDmif.h"

#include <CorbaUtil.h>

//#include <GenericGUIManagedProcess.h>

#include "ReceivedEvent.h"
#include "DebugUtil.h"



#ifdef _DEBUG

#undef THIS_FILE

static char THIS_FILE[]=__FILE__;

#define new DEBUG_NEW

#endif


using namespace TA_Core::TA_Utilities;
using namespace TA_Core::TA_Corba;


//////////////////////////////////////////////////////////////////////

// Construction/Destruction

//////////////////////////////////////////////////////////////////////



CEventReceiver::CEventReceiver()

{

}



CEventReceiver::~CEventReceiver()

{

}



// This method registers this object to listen for events on the system channel

// This method must be called before events can be received

void CEventReceiver::Register() throw (ExEvent&)

{

	LOG(SourceInfo, DebugUtil::GenericLog, SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "CEventReceiver: Register");

	try

	{

        // ANITA: Redo
		//EventListener::registerForEvents(CorbaUtil::EVT_CH_CURRENT, this);

	}



	catch(...)

	{

		throw ExEvent("System error while registering for events");

	}

}



// This does the following:

// 1) determine if the event is a HistoryUpdate

// 2) If it is push the status string onto the safeQ

// 3) Call Notify()

//void CEventReceiver::push(const CORBA::Any& any) throw (ExEvent&)

void CEventReceiver::callback(const CallbackSubmission* submission)

	throw( CORBA::SystemException)

{

    try

	{
        //ANITA: Redo

        /*

  		nexus::EventData* event = NULL;

    

		const ReceivedEvent* re = dynamic_cast<const ReceivedEvent*>(submission);

		// Make sure the cast has succeeded

		if (re != NULL)

		{

			// Check we know the event type

			if ( (*(re->getData()) >>= event ) == 0)

			{

				return;

			}



			// Check the event data is not empty.

			if ( event == 0 )

			{

				LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug, _T( "Received event is empty!" ) );

				return;

			}



			// Determine if this is an History Update event

			if (strcmp(event->eventType.in(), "HistoryUpdate") != 0)

			{

				return; // No, it isn't

			}



			time_t updateTime;



			if( !(event->state >>= updateTime) )

			{

				LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug, "Event does not contain time_t");

				return;

			}



			HistoryUpdate histUpdate;

			histUpdate.historyConfigPkey = event->entityKey;

			histUpdate.updateTime = updateTime;



			// Push the updated sign onto the queue

			LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug, "CEventReceiver: pushing event onto safequeue");

			m_SafeQueue.push(histUpdate);

			

			// Tell the app that a Sign Update message has been received

			Notify();

            

			LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug, "Finished CEventReceiver::push()");

		}
        */
	}



	catch (...)

	{

		LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugFatal, "Exception thrown while receiving event");

	    // ANITA: Redo
        //GenericGUI::CGenericGUIManagedProcess::GetInstance()->UpdateStatus(nexus::ProcessManagerDmIf::DependencyFail);

	}

}



// This has been overridden only because it is pure virtual in the base class

// The method currently does nothing

/*void CEventReceiver::disconnect_push_consumer() throw (ExEvent&)

{

}*/



// This method send a message to the main window indicating that History has changed

void CEventReceiver::Notify()

{

	//AfxGetMainWnd()->PostMessage(WM_HISTORY_UPDATE);

	// Send WM_PUSH message to History Viewer.

	::PostMessage( AfxGetApp()->GetMainWnd()->m_hWnd, WM_HISTORY_UPDATE, NULL, NULL );

}



// Get a HistoryUpdate from the queue.

// Returns true if there is a HistoryUpdate on the queue.

bool CEventReceiver::GetHistoryUpdate(HistoryUpdate& historyUpdate)

{

	return( m_SafeQueue.pop(historyUpdate) );

}





