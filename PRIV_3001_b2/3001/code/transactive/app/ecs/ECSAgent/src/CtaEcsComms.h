/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/CtaEcsComms.h $
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
