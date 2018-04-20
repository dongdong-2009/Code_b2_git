/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/PTTMessagePoster.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  PTTMessagePoster.h
//  Implementation of the Class PTTMessagePoster
//  Created on:      17-Feb-2004 10:10:32 AM
///////////////////////////////////////////////////////////

#if !defined(PTTMessagePoster_H)
#define PTTMessagePoster_H

#include "stdafx.h"
#include "bus/radio/PTTMonitorLibrary/src/IPTTMonitoring.h"
#include <string>

/**
 * This class provides the IPPT used to listen for PTT actions.  It converts these to 
 * windows messages and posts them the the specified windows for processing.
 * It is used to ensure that the GUI thread is used to make the windows updates.
 * @version 1.0
 * @created 17-Feb-2004 10:10:32 AM
 */

namespace TA_IRS_App
{
	class PTTMessagePoster : public TA_IRS_Bus::IPTTMonitoring	              
	{
		public:

			enum PTTAction {PTT_ACTIVATED,PTT_RELEASED,PTT_GRANTED,PTT_QUEUED,PTT_CEASE,PTT_BUSY,PTT_MONITORING};

			/**
			 * Constructor
			 * @param pWnd     Window to receive the posted messages
			 * @param message  Id of the message to be posted to the window
			 * 
			 */
			PTTMessagePoster(CWnd& Wnd, UINT message);
			virtual ~PTTMessagePoster();


			/**
			 * Called when PTT has been activated.  This is used to notify the client
			 * application so it can indicate the activation to the user.  Note:  The right to
			 * talk may not be granted immediatley, so the client should wait for demandTX to
			 * be given before talking.
			 */
			virtual void onPTTActivated();

			/**
			 * Called when PTT has been release.  This method notifies the client to allow GUI
			 * to reflect this change.  The radio will confirm the cease of tx.
			 */
			virtual void onPTTReleased();

			/**
			 * Called when PTT has been granted by the radio
			 */
			virtual void onPTTGranted();

			/**
			 * Called when PTT monitoring cannot be performed as it is already in progress for
			 * another application
			 */
			virtual void onPTTQueued();

			/**
			 * Called when radio confirms TX has ceased. 
			 */
			virtual void onPTTCease();

			/**
			  * onPTTBusy
			  * 
			  * This method is called when the PTT is busy and cannot
			  * be used.
			  */
			virtual void onPTTBusy();

			/**
			  * onPTTMonitoring
			  * 
			  * This method is called when the PTT is ready and monitoring.
			  */
			virtual void onPTTMonitoring();

		private:

			PTTMessagePoster(const PTTMessagePoster& thePoster);

			CWnd&       m_notifyWindow;
			UINT        m_notifyMessageNumber;


	};
};
#endif // !defined(PTTMessagePoster_H)
