/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/TableManager.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last mofified by:  $Author: builder $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */
#ifndef TABLEMANAGER_H
#define TABLEMANAGER_H

#include "core/corba/src/ServantBase.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"

#include "app/ats/AtsAgent/src/AtsWatchDogTable.h"
#include "app/ats/AtsAgent/src/IAtsTable.h"

#include "app/ats/AtsAgent/src/IscsWatchDogTable.h"
#include "app/ats/AtsAgent/src/IIscsTable.h"

#include "app/ats/AtsAgent/src/DataPointHelper.h"
#include "app/ats/AtsAgent/src/DataManager.h"

namespace TA_IRS_App
{
	class ModbusComms;

    class TableManager : public virtual TA_Base_Core::Thread
    {
    public:

        /**
          * Constructor
          */
        TableManager(TA_Base_Core::AtsAgentEntityDataPtr entityData,
					 DataManager& dataManager);

        /**
          * Destructor
          */
        virtual ~TableManager();
        
		
		// Change between Monitor/Control modes
		void setToMonitorMode();
		void setToControlMode();

        void readAtsWatchdog(int connection);
		void connectionFailed(int connection);
        
		//
		//	Thread Methods
		//

		// Define a run() method to do your work. This should block until
		// terminate() is called eg: CorbaUtil::GetInstance().Run() or
		// m_dialog->DoModal()
		//
		// NOTE: do not call this method directly. It needs to be public so that it
		//       can be called from the thread-spawning function
		virtual void run();

		// The terminate() method should cause run() to return. eg:
		// CorbaUtil::GetInstance().Shutdown() or EndDialog(m_dialog)
		//
		// NOTE: probably not a good idea to call this method directly - call
		//       terminateAndWait() instead
		virtual void terminate();

	protected:

		void readAtsTables();
		bool writeIscsTables();
		
		int getConnection();
		int getAnotherConnection();
		int nextConnection();
		int getNumConnections();
        
    private:
        // Disable default constructor, copy constructor and assignment operator
		TableManager();
        TableManager( const TableManager& atsAgent);
        TableManager& operator=(const TableManager &);

	protected:

		AtsWatchDogTable*					m_atsWatchDogTable;
		std::map<std::string, IAtsTable*>	m_atsTables;
											
		IscsWatchDogTable*					m_iscsWatchDogTable;
		std::map<std::string, IIscsTable*>	m_iscsTables;
																						
		std::vector<ModbusComms*>			m_modbusComms;
		int									m_currentConnection;
											
		TA_Base_Core::AtsAgentEntityDataPtr	m_entityData;

		bool								m_keepRunning;

		bool								m_read;
		time_t								m_nextPoll;

		DataManager&						m_dataManager;
    };
}

#endif // !defined(ATSAGENT_H)

