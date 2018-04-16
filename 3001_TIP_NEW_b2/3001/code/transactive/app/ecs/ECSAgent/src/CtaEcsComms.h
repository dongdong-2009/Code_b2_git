/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/CtaEcsComms.h $
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

#if !defined(CtaEcsComms_INCLUDED)
#define CtaEcsComms_INCLUDED

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "app/ecs/ECSAgent/src/ItaEcsComms.h"

class ItaEcsCachedConfig;

namespace TA_Base_Core 
{ 
    class CommsMessageSender; 
}





class CtaEcsComms : public virtual ItaEcsComms
{

public:

    CtaEcsComms(ItaEcsCachedConfig& cachedConfigInstance);
    virtual ~CtaEcsComms();

    virtual void sendEcsStationModeExecutionUpdate(
        TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData* disposableUpdateData,
        const std::string& sessionIdFilter);

    virtual void sendEcsMasterModeExecutionUpdate(
        TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData* disposableUpdateData,
        const std::string& sessionIdFilter);

    virtual void sendEcsProposedModesUpdate(
        TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData* disposableUpdateData);

protected:

    TA_Base_Core::CommsMessageSender*           m_commsSender;
    //TA_Base_Core::ReEntrantThreadLockable       m_lock;
    ItaEcsCachedConfig&                         m_cachedConfigInstance;


};


#endif // !defined(CtaEcsComms_INCLUDED)
