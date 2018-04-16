/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/ActivePlanAgency.h $
  * @author:  Huang Jian
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2017/12/12 14:18:48 $
  * Last modified by:  $Author: hoa.le $
  *
  **/

#ifndef ACTIVEPLANAGENCY_H_INCLUDED
#define ACTIVEPLANAGENCY_H_INCLUDED

#include <map>
#include "ace/Thread_Mutex.h"

#include "bus/response_plans/common/src/IReqHandler.h"

namespace TA_Base_Core
{
    struct ActivePlanDetail;
}

namespace TA_Base_Bus
{
    class PlanAgentLibrary;
    class SortableActiveStepId;
}

namespace TA_Base_App
{
    class InteractiveStep;

	// Class declaration
    class ActivePlanAgency : public TA_Base_Bus::IReqHandler
	{
	// Public methods
    public:
        static ActivePlanAgency& getInstance();
        static void destroyInstance();

        void init();
        void uninit();
        TA_Base_Bus::PlanAgentLibrary* getPlanAgent() const;

        void processActivePlanUpdate( const TA_Base_Core::ActivePlanDetail& refPlanDetail );

        virtual void handle( const unsigned int unId, const int eType, const unsigned int ulLocation,
            const unsigned int ulPlanId, const unsigned int ulInstanceNum, const unsigned int ulStepNum, 
            const std::vector<std::string>& vecParameters );

        void acknowledgeRequest( const unsigned int unId );

	private:
		ActivePlanAgency();
		virtual ~ActivePlanAgency();
		ActivePlanAgency( const ActivePlanAgency& otherActivePlanAgency );
		ActivePlanAgency& operator=( const ActivePlanAgency& otherActivePlanAgency );

        void cleanSteps();

    private:

        typedef std::map<TA_Base_Bus::SortableActiveStepId, InteractiveStep*> ActivePlanStepMap;
        typedef ActivePlanStepMap::iterator ActivePlanStepMapIt;

        ACE_Thread_Mutex m_mtxStepsLock;
        ActivePlanStepMap m_activePlanStepMap;

        TA_Base_Bus::PlanAgentLibrary* m_planAgent;

    private:
        static ActivePlanAgency* s_instance;
        static ACE_Thread_Mutex s_singletonLock;
	}; // ActivePlanAgency
} // TA_Base_App

#endif //ACTIVEPLANAGENCY_H_INCLUDED
