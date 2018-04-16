/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/StationOccUpdateProcessor.h $
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
#ifndef STATIONOCCUPDATEPROCESSOR_H
#define STATIONOCCUPDATEPROCESSOR_H

#include "time.h"

#include <vector>
#include <map>

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"

#include "app/ats/AtsAgent/src/OccUpdateProcessor.h"
#include "app/ats/AtsAgent/src/WatchDogData.h"

namespace TA_IRS_App
{
    class StationOccUpdateProcessor : public TA_Base_Core::Thread
    {
    public:

        /**
          * Constructor
          */
        StationOccUpdateProcessor(TA_Base_Core::CorbaName& corbaName, WatchDogData& watchDogData);

        /**
          * Destructor
          */
        virtual ~StationOccUpdateProcessor();

		//
		// ECS
		//
		void addEcsUpdate(const OccUpdateProcessor::EcsMessage& ecsMessage);

		//
		// Train
		//
		void addTrainUpdate(const OccUpdateProcessor::TrainMessage& trainMessage);
	
		//
		//	CCTV
		//
		void addCctvUpdate(const OccUpdateProcessor::CctvMessage& cctvMessage);

		//
		// TIS
		//
		void addTisUpdate(const OccUpdateProcessor::TisMessage& tisMessage);
		
		//
		// PAS
		//
		void addPasUpdate(const OccUpdateProcessor::PasMessage& pasMessage);

		void processUpdates();

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

	private:
        // Disable default constructor, copy constructor and assignment operator
		StationOccUpdateProcessor();
        StationOccUpdateProcessor( const StationOccUpdateProcessor& occUpdateProcessor);
        StationOccUpdateProcessor& operator=(const StationOccUpdateProcessor &);

	protected:

		//
		// ECS
		//
		typedef std::vector<OccUpdateProcessor::EcsMessage> EcsMessageVector;

		struct OccEcsUpdates
		{
			EcsMessageVector			messages;
			TA_Base_Core::ReEntrantThreadLockable
										lock;
		};

		OccEcsUpdates					m_ecsUpdates;

		//
		// Train
		//
		typedef std::vector<OccUpdateProcessor::TrainMessage> TrainMessageVector;

		struct OccTrainUpdates
		{
			TrainMessageVector			messages;
			TA_Base_Core::ReEntrantThreadLockable
										lock;
		};

		OccTrainUpdates					m_trainUpdates;

	
		//
		//	CCTV
		//
		typedef std::vector<OccUpdateProcessor::CctvMessage> CctvMessageVector;
		
		struct OccCctvUpdates
		{
			CctvMessageVector			messages;
			TA_Base_Core::ReEntrantThreadLockable
										lock;
		};

		OccCctvUpdates					m_cctvUpdates;

		//
		// TIS
		//
		typedef std::vector<OccUpdateProcessor::TisMessage> TisMessageVector;

		struct OccTisUpdates
		{
			TisMessageVector			messages;
			TA_Base_Core::ReEntrantThreadLockable
										lock;
		};

		OccTisUpdates					m_tisUpdates;
		
		//
		// PAS
		//
		typedef std::vector<OccUpdateProcessor::PasMessage> PasMessageVector;

		struct OccPasUpdates
		{
			PasMessageVector			messages;
			TA_Base_Core::ReEntrantThreadLockable
										lock;
		};

		OccPasUpdates					m_pasUpdates;

		OccUpdateProcessor::AtsAgentNamedObject*			
										m_atsAgent;

		TA_Base_Core::CorbaName			m_corbaName;
		WatchDogData&					m_watchDogData;

		TA_Base_Core::Semaphore			m_semaphore;
		volatile bool					m_keepRunning;
    };
}

#endif // !defined(OCCUPDATEPROCESSOR_H)
