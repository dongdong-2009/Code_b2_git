/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/ItaEcsAuditing.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/10/28 18:12:47 $
  * Last modified by:  $Author: builder $
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

    virtual void sendEcsStationModeRequest(/*unsinged*/ long fromMode,	//dahe++ for CL-21046
                                   /*unsigned*/ long toMode,	//dahe++ for CL-21046
                                  const std::string& sessionId)=0;

    virtual void sendEcsStationModeResult(/*unsinged*/ long fromMode,	//dahe++ for CL-21046
                                  /*unsinged*/ long toMode,	//dahe++ for CL-21046
                                  const std::string& failuresIfAny)=0;

    virtual void sendEcsMasterModeRequest(/*unsinged*/ long fromMode,	//dahe++ for CL-21046
                                  /*unsinged*/ long toMode,	//dahe++ for CL-21046
                                  const std::string& sessionId)=0;

    virtual void sendEcsMasterModeResult(/*unsinged*/ long fromMode,	//dahe++ for CL-21046
                                 /*unsinged*/ long toMode,	//dahe++ for CL-21046
                                 const std::string& failuresIfAny,
								 const std::string& sessionId)=0;	//CL17278++ Sheet3

    virtual void sendEcsAgentExecutionState(TA_IRS_Bus::EEcsExecutionState state,
                                    /*unsinged*/ long fromMode,	//dahe++ for CL-21046
                                    /*unsinged*/ long toMode)=0;	//dahe++ for CL-21046
};


#endif // !defined(ItaEcsAuditing_INCLUDED)
