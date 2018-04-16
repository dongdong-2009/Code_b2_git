/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/PTTMessagePoster.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// PTTMessagePoster.cpp: implementation of the PTTMessagePoster class.
//
//////////////////////////////////////////////////////////////////////

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/PTTMessagePoster.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace TA_IRS_App;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
PTTMessagePoster::PTTMessagePoster(CWnd& Wnd, UINT message)
				 : m_notifyWindow(Wnd),
				   m_notifyMessageNumber(message)
{
}

PTTMessagePoster::~PTTMessagePoster()
{
}


/**
 * Called when PTT has been activated.  This is used to notify the client
 * application so it can indicate the activation to the user.  Note:  The right to
 * talk may not be granted immediatley, so the client should wait for demandTX to
 * be given before talking.
 */
void PTTMessagePoster::onPTTActivated()
{
	m_notifyWindow.PostMessage(m_notifyMessageNumber,(LPARAM)PTT_ACTIVATED,0);
}


/**
 * Called when PTT has been release.  This method notifies the client to allow GUI
 * to reflect this change.  The radio will confirm the cease of tx.
 */
void PTTMessagePoster::onPTTReleased()
{
	m_notifyWindow.PostMessage(m_notifyMessageNumber,(LPARAM)PTT_RELEASED,0);
}

/**
 * Called when PTT has been granted by the radio
 */
void PTTMessagePoster::onPTTGranted()
{
	m_notifyWindow.PostMessage(m_notifyMessageNumber,(LPARAM)PTT_GRANTED,0);
}

/**
 * Called when PTT monitoring cannot be performed as it is already in progress for
 * another application
 */
void PTTMessagePoster::onPTTQueued()
{
	m_notifyWindow.PostMessage(m_notifyMessageNumber,(LPARAM)PTT_QUEUED,0);
}

/**
 * Called when radio confirms TX has ceased. 
 */
void PTTMessagePoster::onPTTCease()
{
	m_notifyWindow.PostMessage(m_notifyMessageNumber,(LPARAM)PTT_CEASE,0);
}

/**
  * onPTTBusy
  * 
  * This method is called when the PTT is busy and cannot
  * be used.
  */
void PTTMessagePoster::onPTTBusy()
{
	m_notifyWindow.PostMessage(m_notifyMessageNumber,(LPARAM)PTT_BUSY,0);
}

/**
  * onPTTMonitoring
  * 
  * This method is called when the PTT is ready and monitoring.
  */
void PTTMessagePoster::onPTTMonitoring()
{
	m_notifyWindow.PostMessage(m_notifyMessageNumber,(LPARAM)PTT_MONITORING,0);
}


