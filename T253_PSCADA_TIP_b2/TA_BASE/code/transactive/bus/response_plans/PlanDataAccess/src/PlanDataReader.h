#ifndef PLANDATAREADER_H_INCLUDED
#define PLANDATAREADER_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/response_plans/PlanDataAccess/src/PlanDataReader.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2018/03/13 14:25:58 $
  *  Last modified by:  $Author: hoa.le $
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data.
  */

#include <map>
#include <string>
#include <vector>

#include "ace/Singleton.h"

#include "bus/response_plans/plan_agent/IDL/src/IPlanAgentCorbaDef.h"
#include "bus/response_plans/PlanDataAccess/src/PlanStructures.h"

namespace TA_Base_Bus
{
// Class declaration
// Note that a typedef following the class declaration declares the
// type "PlanDataReader" and that "PlanDataReadingClass" can
// not be used.
typedef std::map<TA_Base_Core::ParameterNumber, std::string> MapParameters;
typedef std::map<unsigned long, TA_Base_Core::StepDetail> StepNumToDetailMap;
typedef std::map<unsigned long, unsigned long> StepNumToStepIdMap;
typedef std::map<std::string, std::string> StepParametersMap;

class PlanDataReadingClass
{
    // Public methods
public:

    friend class ACE_Singleton<PlanDataReadingClass, ACE_Recursive_Thread_Mutex>;

    // CORBA data
    TA_Base_Core::SiblingNodes* getRootNodeChildren( void ) const;
    TA_Base_Core::SiblingNodes* getChildren( unsigned long parent ) const;
    TA_Base_Core::NodeDetail* getNodeDetail( unsigned long node ) const;
    TA_Base_Core::NodeSummary* getNodeSummary( unsigned long node ) const;
    unsigned long getParent( unsigned long node ) const;
    TA_Base_Core::StepNames* getStepNames( void ) const;
    TA_Base_Core::StepDetails* getAllStepDetails( unsigned long plan ) const;
    TA_Base_Core::StepDetails* getSpecifiedStepDetails( unsigned long plan, const TA_Base_Core::StepIds& steps ) const;
    TA_Base_Core::ParameterNames* getParameterNames( const int stepType ) const;
    TA_Base_Core::StepParameters*  getStepParameters( unsigned long plan, unsigned long step, bool isStepNumber = false ) const;
	PlanTemplateStepParameterList getMLParametersByDataPtType() const;
    TA_Base_Core::Steps* getAllSteps( unsigned long plan ) const;
    TA_Base_Core::SequenceFlows* getSequenceFlows( unsigned long plan ) const;
    TA_Base_Core::Plan* getPlan( unsigned long plan );

    std::string getNodeName( unsigned long node ) const;

    unsigned long getStepCount( const unsigned long plan ) const;
    unsigned long getStepNumber( const unsigned long step ) const;
    unsigned long getStepPosition( const unsigned long plan, const unsigned long step ) const;
    unsigned long getStepId( const unsigned long plan, const unsigned long position ) const;
    unsigned long getStepId( const StepNumToStepIdMap& refStepNumToIds, const std::string& stepReference ) const;

    TA_Base_Core::EApprovalState getPlanApprovalState( unsigned long plan ) const;
    TA_Base_Core::ENode getNodeType( unsigned long node ) const;
    std::string getNodePath( unsigned long node ) const;

    TA_Base_Core::NodeIds* getNodeIdsInNodePath( unsigned long plan ) const;

    unsigned long getNodeId( const std::string& path ) const;
    unsigned long getNodeId( unsigned long parent, const std::string& planName );

    bool canUseName( unsigned long node, unsigned long parent, const std::string& name );
    bool existsNode( unsigned long node );

    int getCategoryLevel( unsigned long category );

    int getStepType( unsigned long plan, unsigned long step, bool byStepNumber = false ) const;
    int getPlanType( unsigned long plan ) const;
    void getStepTypes( std::map<unsigned long, StepTypeInfo>& mapTypes );
    void getStepShortcuts( std::map<unsigned long, std::map<unsigned long, StepShortcut> >& mapShortcuts );

    unsigned long getNextPlanSeqNumber( void );

    std::map<unsigned long, int> getPlanTypeMap( void );

    // Private methods
private:
    PlanDataReadingClass();
    ~PlanDataReadingClass() {}

    PlanDataReadingClass( const PlanDataReadingClass& );
    PlanDataReadingClass& operator=( const PlanDataReadingClass& );

    TA_Base_Core::StepDetails* getStepDetails( unsigned long plan, const std::string& stepConstraint ) const;

    std::string getNvlString( const std::string& strSrc ) const;
    unsigned long getPlanId( const std::string& planReference ) const;
    unsigned long getStepId( unsigned long plan, const std::string& stepReference ) const;

    unsigned long stringToUlong( const std::string& strSrc ) const;

    // Private data
private:
    const unsigned long m_rootNodeId;

    std::string m_nvlValue;

}; // PlanDataReadingClass

typedef ACE_Singleton<PlanDataReadingClass, ACE_Recursive_Thread_Mutex> PlanDataReader;

} // TA_Base_Bus

#endif //PLANDATAREADER_H_INCLUDED
