/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/ItaEcsAgentCompleteState.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// ItaEcsAgentCompleteState.h: interface for the ItaEcsAgentCompleteState class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(ItaEcsAgentCompleteState_INCLUDED)
#define ItaEcsAgentCompleteState_INCLUDED

#include "bus/ecs/ecs_agent/IDL/src/IStationEcsAgentCorbaDef.h"
#include "bus/ecs/ecs_agent/IDL/src/IMasterEcsAgentCorbaDef.h"
#include "bus/ecs/ecs_agent/IDL/src/CommonEcsAgentCorbaDef.h"
#include "app/ecs/ECSAgent/src/EcsGlobals.h"


class ItaEcsAgentCompleteState
{

public:
    
    ///////////////////////////////////////////////////////////////////////////////
    //
    // Set Methods
    //
    ///////////////////////////////////////////////////////////////////////////////


    virtual void setDataPoints(DataPoints& dataPoints)=0;
    virtual void setStationModeState(const TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& stationModeState)=0;
    virtual void setMasterModeState(const TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData& masterModeState)=0;

    virtual void commitProposedModes()=0;

    // Modifiers for ProposedModes
    virtual void addProposedMode(const TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode& mode)=0;
    virtual void removeProposedMode(const TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode& mode)=0;    
    virtual void clearProposedModes(TA_IRS_Bus::IMasterEcsAgentCorbaDef::EProposedModeType modeType)=0;
	virtual void clearProposedOverridingStationModes(unsigned long locationKey)=0;
    virtual void clearProposedStationModes(unsigned long locationKey)=0;


    ///////////////////////////////////////////////////////////////////////////////
    //
    // Get Methods
    //
    ///////////////////////////////////////////////////////////////////////////////

        
    virtual void getStationModeState(TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& returnValue)=0;
    // Just the execution state enum
    virtual void getStationModeState(TA_IRS_Bus::EEcsExecutionState& returnValue)=0;
    virtual void getMasterModeState(TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData& returnValue)=0;

    virtual void getProposedModes(TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes& returnValue)=0;
    virtual void getProposedModeSet(ProposedModeSet& returnValue)=0;
    virtual void getProposedModeSet(
        unsigned long locationKey, 
        TA_IRS_Bus::IMasterEcsAgentCorbaDef::EProposedModeType modeType,
        ProposedModeSet& returnValue)=0;
};


#endif // !defined(ItaEcsAgentCompleteState_INCLUDED)





