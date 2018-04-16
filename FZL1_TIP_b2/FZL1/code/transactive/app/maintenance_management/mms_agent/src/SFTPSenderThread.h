
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/maintenance_management/mms_agent/src/SFTPSenderThread.h $
  * @author:   huirong.luo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  * 
  * This class get MMS request message from database and send to MMS server regularly.
  */

#if !defined(EA_EB3C106E_47C3_458e_A221_0409BE74E903__INCLUDED_)
#define EA_EB3C106E_47C3_458e_A221_0409BE74E903__INCLUDED_

#include <vector>
#include "core/threads/src/Thread.h"
#include "core/data_access_interface/entity_access/src/MmsAgentEntityData.h"
#include "core/sftp/src/SFTPManager.h"
#include "bus/maintenance_management/mmsHelper/src/IMMSAPI.h"

namespace TA_Base_Core
{
	class MmsRequestAlarmData;
	class MmsRequestSchematicData;
	class  MmsRequestScheduledData;	 
}

namespace TA_IRS_App
{
	   
	class SFTPSenderThread : public TA_Base_Core::Thread
	{
	public:

		SFTPSenderThread(TA_Base_Core::MmsAgentEntityDataPtr pData);

		virtual ~SFTPSenderThread();

		/** Define a run() method to do your work. This should block until
		  * terminate() is called eg: CorbaUtil::GetInstance().Run() or
		  * m_dialog->DoModal()
		  *
		  * NOTE: do not call this method directly. It needs to be public so that it
		  *       can be called from the thread-spawning function
		 */
		virtual void run();

		/* The terminate() method should cause run() to return. eg:
		  * CorbaUtil::GetInstance().Shutdown() or EndDialog(m_dialog)
		  *
		  * NOTE: probably not a good idea to call this method directly - call
		  *       terminateAndWait() instead
		 */
		virtual void terminate();       

		void setScanPeriodSecond(unsigned int sec);        
 
		void setToControlMode();
		 
		void setToMonitorMode();

		void sendMMSMessages();
		 
	private: //data
		bool m_stopRequest;
    	//online config params
		long m_scanPeriodSeconds;	 	 
    
		TA_Base_Core::ReEntrantThreadLockable  m_dataLock;
       
		bool m_isControlMode;

		int setBuffer(char* buffer, std::string& data, int maxlength);
		void setBuffer(char* buffer, timeb& data, int maxlength);
		void setBuffer(char* buffer, unsigned long data, int maxlength);
		void setBuffer(char* buffer, double data, int maxlength);

		bool getAutoAlarmMessage(char* msgContext,int& startPosition, unsigned long  msgIndex ,TA_Base_Core::MmsRequestAlarmData* data);
		bool getSemiAutoAlarmMessage(char* msgContext,int& startPosition, unsigned long  msgIndex ,TA_Base_Core::MmsRequestAlarmData* data);
		 
		bool getRequestSchematicMessage(char* msgContext,int& startPosition, unsigned long  msgIndex ,TA_Base_Core::MmsRequestSchematicData* data);
		bool getCountMessage(char* msgContext,int& startPosition, unsigned long  msgIndex ,TA_Base_Core::MmsRequestScheduledData* data);
		bool getMeasureMessage(char* msgContext,int& startPosition, unsigned long  msgIndex ,TA_Base_Core::MmsRequestScheduledData* data);
		bool createMessageHeader(char* msgContext,int startPosition, unsigned long  msgIndex, TA_Base_Bus::MMSMessageType::MessageType msgType);
		
		std::string getNewMessageFileName();
		bool sendbySFTP(char* msgContext, int length);

		void deleteDatabaseData(std::vector<TA_Base_Core::MmsRequestAlarmData*>& vecAlarm,
				std::vector<TA_Base_Core::MmsRequestSchematicData*>& vecSchematic,
				std::vector<TA_Base_Core::MmsRequestScheduledData*>& vecScheduled);

		//SFTP configuration
		std::string m_SftpHostname;
		unsigned long m_SftpPort;
		std::string m_SftpUserName;
		std::string m_SftpPassword;
		std::string m_SftpDestinationPath;
		std::string m_SftpSourcePath;

		//Character set configuration
		std::string m_CharacterSet;

		//FTP File Transfer Mode
		TA_Base_Core::FTPClient::FileType m_FTPFileTransferMode;
		
		//use to create a unique file name
		unsigned long  m_FileNumber;
	    timeb    m_PreviousFileTime;
	};
} //end TA_App

#endif  // EA_EB3C106E_47C3_458e_A221_0409BE74E903__INCLUDED_
