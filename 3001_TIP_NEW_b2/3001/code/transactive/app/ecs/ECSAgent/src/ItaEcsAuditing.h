/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/ItaEcsAuditing.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// CachedConfig.h: interface for the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(ItaEcsAuditing_INCLUDED)
#define ItaEcsAuditing_INCLUDED

#include "bus/ecs/ecs_agent/IDL/src/CommonEcsAgentCorbaDef.h"


class ItaEcsAuditing  
{

public:

    virtual void sendEcsAgentOnlineUpdate(const std::string& parameter)=0;

    virtual void sendEcsStationModeRequest(unsigned long fromMode,
                                   unsigned long toMode,
                                  const std::string& sessionId)=0;

    virtual void sendEcsStationModeResult(unsigned long fromMode,
                                  unsigned long toMode,
                                  const std::string& failuresIfAny)=0;

    virtual void sendEcsMasterModeRequest(unsigned long fromMode,
                                  unsigned long toMode,
                                  const std::string& sessionId)=0;

    virtual void sendEcsMasterModeResult(unsigned long fromMode,
                                 unsigned long toMode,
                                 const std::string& failuresIfAny,
								 const std::string& sessionId)=0;	//CL17278++ Sheet3

    virtual void sendEcsAgentExecutionState(TA_IRS_Bus::EEcsExecutionState state,
                                    unsigned long fromMode,
                                    unsigned long toMode)=0;
};


#endif // !defined(ItaEcsAuditing_INCLUDED)
