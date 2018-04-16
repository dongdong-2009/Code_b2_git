/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/CtaEcsAuditing.h $
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

#if !defined(CtaEcsAuditing_INCLUDED)
#define CtaEcsAuditing_INCLUDED

#include <set>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "app/ecs/ECSAgent/src/ItaEcsAuditing.h"

class ItaEcsCachedConfig;
namespace TA_Base_Core 
{ 
    class AuditMessageSender; 
}

class CtaEcsAuditing : public virtual ItaEcsAuditing
{

public:

    CtaEcsAuditing(ItaEcsCachedConfig& cachedConfigInstance);
    virtual ~CtaEcsAuditing();

    virtual void sendEcsAgentOnlineUpdate(const std::string& parameter);

    virtual void sendEcsStationModeRequest(/*unsigned*/ long fromMode,	//dahe++ for CL-21046
                                   /*unsigned*/ long toMode,		//dahe++ for CL-21046
                                  const std::string& sessionId);

    virtual void sendEcsStationModeResult(/*unsigned*/ long fromMode,		//dahe++ for CL-21046
                                  /*unsigned*/ long toMode,		//dahe++ for CL-21046
                                  const std::string& failuresIfAny);

    virtual void sendEcsMasterModeRequest(/*unsigned*/ long fromMode,		//dahe++ for CL-21046
                                  /*unsigned*/ long toMode,		//dahe++ for CL-21046
                                  const std::string& sessionId);

    virtual void sendEcsMasterModeResult(/*unsigned*/ long fromMode,		//dahe++ for CL-21046
                                 /*unsigned*/ long toMode,		//dahe++ for CL-21046
                                 const std::string& failuresIfAny, 
								 const std::string& sessionId);		//CL17278++: Sheet3

    virtual void sendEcsAgentExecutionState(TA_IRS_Bus::EEcsExecutionState state,
                                    /*unsigned*/ long fromMode,		//dahe++ for CL-21046
                                    /*unsigned*/ long toMode);		//dahe++ for CL-21046


protected:

    TA_Base_Core::AuditMessageSender*               m_auditSender;
    //TA_Base_Core::ReEntrantThreadLockable           m_lock;
    ItaEcsCachedConfig&                             m_cachedConfigInstance;
};


#endif // !defined(CtaEcsAuditing_INCLUDED)
