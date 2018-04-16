#ifndef PLANDATAREADER_H_INCLUDED
#define PLANDATAREADER_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/bus/response_plans/PlanDataAccess/src/PlanDataReader.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data.
  */
#include "bus/response_plans/plan_agent/IDL/src/IPlanAgentCorbaDef.h"

namespace TA_Base_Bus
{
	// Class declaration
	// Note that a typedef following the class declaration declares the
	// type "PlanDataReader" and that "PlanDataReadingClass" can
    // not be used.
    typedef std::map<TA_Base_Core::ParameterNumber, std::string> MapParameters;
    typedef std::map<TA_Base_Core::StepNumber, TA_Base_Core::StepDetail> StepNumToDetailMap;
	
	class PlanDataReadingClass
	{
	// Public methods
	public:

		friend class ACE_Singleton<PlanDataReadingClass, ACE_Recursive_Thread_Mutex>;

		// CORBA data
		TA_Base_Core::SiblingNodes*      getRootNodeChildren( void ) const;
		TA_Base_Core::SiblingNodes*      getChildren( TA_Base_Core::NodeId parent ) const;
		TA_Base_Core::NodeDetail*        getNodeDetail( TA_Base_Core::NodeId node ) const;
		TA_Base_Core::NodeSummary*		getNodeSummary ( TA_Base_Core::NodeId node ) const;
		TA_Base_Core::NodeId				getParent( TA_Base_Core::NodeId node ) const;
		TA_Base_Core::StepNames*         getStepNames( void ) const;
		TA_Base_Core::StepDetails*       getAllStepDetails( TA_Base_Core::NodeId plan ) const;
		TA_Base_Core::StepDetails*       getSpecifiedStepDetails( TA_Base_Core::NodeId plan, const TA_Base_Core::StepIds& steps ) const;
		TA_Base_Core::ParameterNames*    getParameterNames( TA_Base_Core::EStep stepType ) const;
        TA_Base_Core::StepParameters*    getStepParameters( TA_Base_Core::NodeId plan, TA_Base_Core::StepId step, bool isStepNumber = false ) const;
		TA_Base_Core::Steps* getAllSteps( TA_Base_Core::NodeId plan ) const;
		TA_Base_Core::Plan*              getPlan( TA_Base_Core::NodeId plan );

		std::string					getNodeName(TA_Base_Core::NodeId node) const;

		
		TA_Base_Core::StepNumber getStepCount( TA_Base_Core::NodeId plan ) const;

		TA_Base_Core::StepNumber getStepNumber( TA_Base_Core::StepId step) const;
        TA_Base_Core::StepId getStepId( TA_Base_Core::NodeId plan, TA_Base_Core::StepNumber step) const;
        TA_Base_Core::StepId getStepId( const StepNumToDetailMap& refStepDetails, const std::string& stepReference ) const;

		TA_Base_Core::EApprovalState getPlanApprovalState(TA_Base_Core::NodeId plan) const;

		TA_Base_Core::ENode getNodeType( TA_Base_Core::NodeId node ) const;
		std::string getNodePath( TA_Base_Core::NodeId node ) const;
        //std::vector<TA_Base_Core::NodeId>* getNodeIdsInNodePath( TA_Base_Core::NodeId plan ) const;
        TA_Base_Core::NodeIds* getNodeIdsInNodePath( TA_Base_Core::NodeId plan ) const;

        TA_Base_Core::NodeId getNodeId(const std::string& path) const;
		TA_Base_Core::NodeId getNodeId( TA_Base_Core::NodeId parent, const std::string& planName );

		bool canUseName(TA_Base_Core::NodeId node, TA_Base_Core::NodeId parent, const std::string& name);
		bool existsNode(TA_Base_Core::NodeId node);

		int getCategoryLevel(TA_Base_Core::NodeId category);

		TA_Base_Core::EStep getStepType( TA_Base_Core::NodeId plan, TA_Base_Core::StepId step, bool byStepNumber = false ) const;

		unsigned long getNextPlanSeqNumber(void);


	// Private methods
	private:
		PlanDataReadingClass();
		~PlanDataReadingClass(){}

		PlanDataReadingClass( const PlanDataReadingClass& );
		PlanDataReadingClass& operator=( const PlanDataReadingClass& );

		TA_Base_Core::StepDetails* getStepDetails( TA_Base_Core::NodeId plan, const std::string& stepConstraint ) const;
		TA_Base_Core::DataPointValue buildDataPointValue( const std::string& dbString ) const;
        TA_Base_Core::WindowPosition getWindowPos( const std::string& dbString ) const;

		TA_Base_Core::IDatabase * getDatabase() const;

        std::string getStringData(std::auto_ptr<class TA_Base_Core::IData>& data, unsigned long row, const std::string& columnName) const;
        TA_Base_Core::NodeId getPlanId(const std::string& planReference) const;
        TA_Base_Core::StepId getStepId(TA_Base_Core::NodeId plan, const std::string& stepReference) const;
        
        void convertStepParameters( TA_Base_Core::StepParameters& refStepParameterrs,
            const StepNumToDetailMap& refStepDetails, const TA_Base_Core::StepId step, 
            const TA_Base_Core::EStep eType, MapParameters& mapParameters ) const;

        unsigned long stringToUlong( const std::string& strSrc ) const;

    // Private data
    private:
        const TA_Base_Core::NodeId m_rootNodeId;

        std::string m_nvlValue;

	}; // PlanDataReadingClass

	typedef ACE_Singleton<PlanDataReadingClass, ACE_Recursive_Thread_Mutex> PlanDataReader;

} // TA_Base_Bus

#endif //PLANDATAREADER_H_INCLUDED
