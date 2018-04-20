/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/radio/PTTMonitorLibrary/src/PTTButton.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

///////////////////////////////////////////////////////////
//  PTTButton.h
//  Implementation of the Class PTTButton
//  Created on:      17-Feb-2004 10:10:32 AM
///////////////////////////////////////////////////////////

#if !defined(PTTButton_5E05CF7E_AAD3_4aa9_A96A_8960E6129774__INCLUDED_)
#define PTTButton_5E05CF7E_AAD3_4aa9_A96A_8960E6129774__INCLUDED_

#include "core\message\src\SpecialisedMessageSubscriber.h"
#include "core\message\idl\src\CommsMessageCorbaDef.h"
#include "bus\radio\RadioAgentCorbaDef_4669\idl\src\IRadioSessionCorbaDef.h"
#include "core\naming\src\NamedObject.h"
#include "IRS232StatusChange.h"
#include "IPTTMonitoring.h"
#include <string>

/**
 * This class handles the monitoring of the microphone PTT button and the
 * notification of change to the radio session remote object and the client
 * interface.
 * @version 1.0
 * @created 17-Feb-2004 10:10:32 AM
 */

namespace TA_IRS_Bus
{
	class RS232MonitorThread;

	class PTTButton : public IRS232StatusChange,
		              public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
	{
		public:

			/**
			 * Constructor
			 * @param notify    Interface used to notify user of change
			 * 
			 */
			PTTButton(TA_IRS_Bus::IPTTMonitoring& notify);
			virtual ~PTTButton();

		public:

			/**
			 * Will need to connect to the RS232 port and monitor radio session messages (for
			 * DemandTX events).  It will also need to access the RadioSession remote object
			 * in order to send the PTT requests.
			 * @param sessionId
			 * 
			 * Throws PTTButtonException.
			 */
			void startMonitoring(long callID,std::string sessionId,std::string comPort);

			/**
			 * release RS232 and RadioSession object, stop subscription to RadioSession events.
			 * 
			 */
			void endMonitoring();

			/**
			 * Indicates if the class is currently monitoring the button.
			 *
			 */
			bool isMonitoring();

			//*******************************
			// IRS232StatusChange Interface *
			//*******************************

			/**
			 * onPortReady  This method is called when the communications port is ready and
			 * can be connected to
			 */
			void onPortReady();
			/**
			 * onPortBusy  This method is called when the communications port is busy and
			 * cannot be connected to.
			 */
			void onPortBusy();
			/**
			 * onCTSEnable  This method is called when the CTS changes from disabled to
			 * enabled
			 */
			void onCTSEnable();
			/**
			 * onCTSDisable  This method is called when the CTS changes from enabled to
			 * disabled
			 */
			void onCTSDisable();


		    void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);


		private:

			PTTButton(const PTTButton& thePTTButton);


			std::string getRadioSessionEntityName(unsigned long locationKey,unsigned long consoleKey);
			void startRadioPTTSubscription(unsigned long locationKey);
			void endRadioPTTSubscription();



			// Session used for logging 
			std::string m_sessionID;

			
			TA_IRS_Bus::RS232MonitorThread*   m_rs232Monitor;

			/**
			 * Used to callback the client and advise on PTT status changes.
			 */
			TA_IRS_Bus::IPTTMonitoring& m_callback;

			// Transaction Id for queued item (0=none queued)
			unsigned long			m_queuedID;


			// Radio Session object 
			typedef TA_Base_Core::NamedObject<TA_Base_Bus::IRadioSessionCorbaDef,
				                         TA_Base_Bus::IRadioSessionCorbaDef_ptr,
										 TA_Base_Bus::IRadioSessionCorbaDef_var> RadioSession;
			
			RadioSession   m_radioSession;


	};
};
#endif // !defined(PTTButton_5E05CF7E_AAD3_4aa9_A96A_8960E6129774__INCLUDED_)