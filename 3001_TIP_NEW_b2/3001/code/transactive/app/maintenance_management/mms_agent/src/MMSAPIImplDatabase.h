/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/3001/transactive/bus/maintenance_management/mmsHelper/src/MMSAPIImplDatabase.h $
* @author:  huirong.luo
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2011/09/29 14:12:57 $
* Last modified by:  $Author: grace.koh $
* 
*Implementation of IMMSAPI
*  .
*
*/
 
#if !defined(EA_738F7F65_8FD7_4989_B2EE_91EBC6145957__INCLUDED_)
#define EA_738F7F65_8FD7_4989_B2EE_91EBC6145957__INCLUDED_
 
#include <string>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/maintenance_management/mmsHelper/src/IMMSAPI.h"
#include "app/maintenance_management/mms_agent/src/SFTPSenderThread.h"

namespace TA_IRS_App
{			  
	class MMSAPIImplDatabase: public  TA_Base_Bus::IMMSAPI
	{
		public:		
			MMSAPIImplDatabase(void);
			virtual ~MMSAPIImplDatabase(void);
			/**
			* submitAutoAlarmMessage
			*
			* use to submit auto alarm MMS request message to MMS
			*
			* @param MmsRequestAlarmData - MMS request message information
			* 
			* @return bool - true means submit successfully, false: submit failed.
			*                  could be determined.
			*/
			virtual bool submitAutoAlarmMessage(TA_Base_Core::MmsRequestAlarmData& alarm);
	 
			virtual bool submitJobRequestMessage(const TA_Base_Bus::JobRequestDetailCorbaDef& details);

			virtual bool submitScheduledMessage(TA_Base_Bus::MMSScheduledMessage& msg);

			virtual bool isMMSLive();

			virtual void initConfiguration(TA_Base_Core::MmsAgentEntityDataPtr pData);

			virtual void setToControlMode();

			virtual void setToMonitorMode();

			

	private:

			bool pingServer(double maxpingsecs);

			void initPing();

			TA_Base_Core::ReEntrantThreadLockable m_lockInitPing;
			bool m_isInitPing;
			bool m_isControlMode;

			SFTPSenderThread*   m_sftpSenderThread; //get MMS message from database and send to MMS server;

			unsigned short m_pid;
			//the singleton object
		 
			//server address
			std::string m_serverAddress;
		 
			
		};
	 
}

#endif // !defined(EA_738F7F65_8FD7_4989_B2EE_91EBC6145957__INCLUDED_)