/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/radio/PTTMonitorLibrary/src/IPTTMonitoring.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

///////////////////////////////////////////////////////////
//  IPTTMonitoring.h
//  Implementation of the Interface IPTTMonitoring
//  Created on:      17-Feb-2004 10:11:44 AM
///////////////////////////////////////////////////////////

#if !defined(IPTTMonitoring_E06A82D3_C6D4_4823_9B57_37BCCC1F2442__INCLUDED_)
#define IPTTMonitoring_E06A82D3_C6D4_4823_9B57_37BCCC1F2442__INCLUDED_

/**
 * This class defines the methods used to advise the client of change.
 * @version 1.0
 * @created 17-Feb-2004 10:11:44 AM
 */
namespace TA_IRS_Bus
{
	class IPTTMonitoring
	{
		public:
			/**
			 * Called when PTT has been activated.  This is used to notify the client
			 * application so it can indicate the activation to the user.  Note:  The right to
			 * talk may not be granted immediatley, so the client should wait for demandTX to
			 * be given before talking.
			 */
			virtual void onPTTActivated() =0;

			/**
			 * Called when PTT has been release.  This method notifies the client to allow GUI
			 * to reflect this change.  The radio will confirm the cease of tx.
			 */
			virtual void onPTTReleased() =0;

			/**
			 * Called when PTT has been granted by the radio
			 */
			virtual void onPTTGranted() =0;

			/**
			 * Called when PTT monitoring cannot be performed as it is already in progress for
			 * another application
			 */
			virtual void onPTTQueued() =0;

			/**
			 * Called when radio confirms TX has ceased. 
			 */
			virtual void onPTTCease() =0;

			/**
			  * onPTTBusy
			  * 
			  * This method is called when the PTT is busy and cannot
			  * be used.
			  */
			virtual void onPTTBusy() = 0;

			/**
			  * onPTTMonitoring
			  * 
			  * This method is called when the PTT is ready and monitoring.
			  */
			virtual void onPTTMonitoring() = 0;


	};
}; // Namespace

#endif // !defined(IPTTMonitoring_E06A82D3_C6D4_4823_9B57_37BCCC1F2442__INCLUDED_)