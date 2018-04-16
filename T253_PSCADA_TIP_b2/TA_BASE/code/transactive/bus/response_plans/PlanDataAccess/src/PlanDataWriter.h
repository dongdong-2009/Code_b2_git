#ifndef PLAN_DATA_DB_WRITER_HEADER_INCLUDED
#define PLAN_DATA_DB_WRITER_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/response_plans/PlanDataAccess/src/PlanDataWriter.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2017/12/12 14:18:48 $
  *  Last modified by:  $Author: hoa.le $
  *
  *  A thread-safe Singleton factory for updating database and adding into database.
  *  
  */
//

#include <string>
#include <vector>

#include "ace/Singleton.h"
#include "ace/RW_Mutex.h"

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
    class IDatabase;
}

namespace TA_Base_Bus
{
	// Class declaration
	// Note that a typedef following the class declaration declares the
	// type "PlanDataWriter" and that "PlanDataDBWriter" can
	// not be used.

    
	class PlanDataWriterImp
	{
    public:
        // Support dynamic language switch
        static std::string CopyNameSuffix;
        static std::string NewPlanName;
        static std::string NewCategoryName;

	// Public methods
	public:

		friend class ACE_Singleton<PlanDataWriterImp, ACE_Recursive_Thread_Mutex>;

        //void setPlanApprovalState(const std::string& session, unsigned long nodeId, TA_Base_Core::EApprovalState approvalState);

		std::string getOperatorName(const std::string& session);

        void UpdatePlansDBSequence();

	//Plan Edit methods

        //void getEditPlanLock( const std::string& session, const std::string& planManagerId, unsigned long plan, bool noDBAccess = false );
		//void releaseEditPlanLock(const std::string& session,unsigned long plan, bool noDBAccess = false );
		bool isPlanBeingEdited(unsigned long plan);
        std::string setEditLockInDataBase (const std::string& session, unsigned long plan );
        void releaseEditLockInDatabase(const std::string& session, unsigned long plan );

        std::string getPlanEditSession(unsigned long plan, const std::string& session);

		//void validatePlan(const TA_Base_Core::Plan& plan);

		//void deleteCategory(const std::string& session, unsigned long category);
		unsigned long insertCategory(const std::string& session, unsigned long parent);
		//void updateCategory(const std::string& session, const TA_Base_Core::CategoryDetail& detail);

		void updatePlanDetail(const std::string& session, const TA_Base_Core::PlanDetail& detail);

		unsigned long insertPlan(const std::string& session, const unsigned long parent, TA_Base_Core::ENode nodeType);
		//void deletePlans(const std::string& session, const unsigned longs& plans);
		//void movePlan(const std::string& session, unsigned long fromPlan, unsigned long toParent, bool useForce);
		//void copyPlan(const std::string& session, unsigned long plan, unsigned long parent, bool useForce );
		/////////////////////////////////////////////////////////////
		//Please note that the name for exportPlan may be a bit misleading 
		//as this method only loads relevant plan details
		//into a structure suitable for export to a .pln file
		//the actual export (file writing happens on manager's side
		/////////////////////////////////////////////////////////////
		TA_Base_Core::AuxPlanDetail* exportPlan (const std::string& session, unsigned long& planId);
		unsigned long importPlan (const std::string& session, const TA_Base_Core::AuxPlanDetail& plan, unsigned long parentId);

        bool insertPlanType( const std::string& session, const unsigned long plan, TA_Base_Core::EPlanType planType );
        void updatePlanType(unsigned long nodeId, TA_Base_Core::EPlanType visibleState);
        void deletePlanType(unsigned long nodeId);

        void deleteAllStepParameters( const std::string& session, const unsigned int plan );
        void deleteAllSteps( const std::string& session, const unsigned int plan );
        void deleteAllSequenceFlows( const std::string& session, const unsigned int plan );
        void insertStepDetail( const unsigned int plan, const TA_Base_Core::StepDetail& detail );
        void insertSequenceFlow( const unsigned int plan, const TA_Base_Core::SequenceFlow& detail );
        void insertStepParameters( const unsigned int stepKey, const std::vector<std::string>& parameters );
        void replacePlan(const std::string& session, const unsigned long existingPlan, const unsigned long planToOverwrite);
        void copyPlan(const std::string& session, const unsigned long planToCopy, const unsigned long parent, const std::string planNewName);
        void updateNodeParent(const std::string& session, const unsigned long parent, const unsigned long plan);
        void deletePlan(const std::string& session, const unsigned long plan);
        void updateCategory(const std::string& session, const std::string& name, const std::string& description, const unsigned long category);
        void deleteCategory(const unsigned long category);
        void setPlanApprovalState(const std::string& session, const unsigned long plan, TA_Base_Core::EApprovalState approvalState);

        /** 
          * getEditLockOwner
          *
          * Returns the session that has owns the edit lock on the plan
          *
          * @param plan The node Id of the plan
          *
          * @return The session Id
          */
        std::string getEditLockOwner(unsigned long plan);
        std::string getNewNodeName(unsigned long parent, TA_Base_Core::ENode nodeType);
        std::string getNodeNameToCopy(unsigned long node, unsigned long parent);

	// Private methods
	private:
		PlanDataWriterImp();
		~PlanDataWriterImp(){}

		PlanDataWriterImp( const PlanDataWriterImp& );
		PlanDataWriterImp& operator=( const PlanDataWriterImp& );

		std::string parseForApostrophe(const std::string& inString);

		TA_Base_Core::IDatabase* getDatabase() const;

        //unsigned long getPlanToReplace(const std::string& planName, unsigned long parent, bool useForce);

        unsigned long getStepNumber(const TA_Base_Core::Plan& plan, unsigned long step) const;

    // Private data
    private:
		mutable ACE_RW_Mutex m_planInsertLock;
	}; // PlanDataDBWriter

	typedef ACE_Singleton<PlanDataWriterImp, ACE_Recursive_Thread_Mutex> PlanDataWriter;

} // TA_Base_Bus

#endif //PLAN_DATA_DB_WRITER_HEADER_INCLUDED
