/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/ItaEcsComms.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// CachedConfig.h: interface for the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(ItaEcsComms_INCLUDED)
#define ItaEcsComms_INCLUDED

#include "bus/ecs/ecs_agent/IDL/src/IMasterEcsAgentCorbaDef.h"


class ItaEcsComms  
{

public:

    virtual void sendEcsStationModeExecutionUpdate(
        TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData* disposableUpdateData,
        const std::string& sessionIdFilter)=0;

    virtual void sendEcsMasterModeExecutionUpdate(
        TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData* disposableUpdateData,
        const std::string& sessionIdFilter)=0;

    virtual void sendEcsProposedModesUpdate(
        TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData* disposableUpdateData)=0;

};


#endif // !defined(ItaEcsComms_INCLUDED)
