/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/OperationModeManager.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #3 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2012/06/01 15:51:29 $
  * Last mofified by:  $Author: peter.wong $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */
#ifndef OPERATIONMODEMANAGER_H
#define OPERATIONMODEMANAGER_H

#include "bus/trains/TrainCorbaProxy/src/TrainOperationModeCorbaProxy.h"

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/naming/src/FlexibleTimeoutNamedObject.h"
#include "core/threads/src/Thread.h"

#include "core/threads/src/ThreadPoolSingletonManager.h"

// forward declarations
namespace TA_Base_Core
{
	class ThreadPoolSingletonManager;
}

namespace TA_IRS_App
{

    class OperationModeManager : public TA_Base_Core::Thread
    {
    public:

        /**
          * Constructor
          */
        OperationModeManager(TA_Base_Core::AtsAgentEntityDataPtr entityData);

        /**
          * Destructor
          */
        virtual ~OperationModeManager();

		enum EOccAtsState
		{
			Startup,
			Normal,
			DoubleFailure
		};

		void inControlMode(bool controlMode);
		bool inControlMode();

		void inIscsDegraded(bool iscsDegraded);
		void inRadioFallback(bool radioFallback);
		void setOccAtsState(EOccAtsState occAtsState);
		void resetAts();

		enum EAgentState
		{
			AgentNormal,
			AgentRadioFallback,
			AgentOccFailure,
			AgentIscsDegraded
		};

		EAgentState getAgentState();
		bool inOccAtsStartup();
		bool inDoubleAtsFailure();

	protected:

		void initialiseTrainAgentNamedObject();

		bool inIscsDegraded();
		bool inRadioFallback();

		void run();
		void terminate();

	private:

		OperationModeManager();

		/**
         * The thread pool used to queue work items
         */
        TA_Base_Core::ThreadPoolSingletonManager* m_threadPool;
	protected:

		enum EControlMode
		{
			UnknownMode,
			ControlMode,
			MonitorMode
		};
			
		EControlMode m_controlMode;
		const EControlMode DEFAULT_CONTROL_MODE; 

		enum EIscsDegraded
		{
			UnknownDegraded,
			IscsNormal,
			IscsDegraded
		};

		EIscsDegraded		m_iscsDegraded;
		const EIscsDegraded	DEFAULT_ISCS_DEGRADED;

		enum ERadioFallback
		{
			UnknownFallback,
			RadioNormal,
			RadioFallback
		};

		ERadioFallback			m_radioFallback;
		const ERadioFallback	DEFAULT_RADIO_FALLBACK;

		EOccAtsState m_occAtsState;

		TA_Base_Core::ReEntrantThreadLockable m_lock;

		unsigned long m_locationKey;
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType m_locationType;

            TA_IRS_Bus::TrainOperationModeCorbaProxyPtr m_trainAgent;


            volatile bool m_keepRunning;
    };

}

#endif // !defined(OPERATIONMODEMANAGER_H)

