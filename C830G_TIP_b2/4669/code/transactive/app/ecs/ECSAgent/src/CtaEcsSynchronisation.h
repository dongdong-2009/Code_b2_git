/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/CtaEcsSynchronisation.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// CachedConfig.h: interface for the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(CtaEcsSynchronisation_INCLUDED)
#define CtaEcsSynchronisation_INCLUDED

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "app/ecs/ECSAgent/src/ItaEcsSynchronisation.h"

namespace TA_Base_Core 
{ 
    class StateUpdateMessageSender; 
}

class CtaEcsSynchronisation : public virtual ItaEcsSynchronisation 
{

public:

    CtaEcsSynchronisation(ItaEcsCachedConfig& m_ecsCachedConfigInstance);
    virtual ~CtaEcsSynchronisation();

    virtual void sendEcsAgentStateUpdateRequest();

    virtual void sendEcsAgentParameterUpdate( TA_IRS_Bus::EEcsAgentParameter parameter, 
                                      const CORBA::Any& parameterData,
                                      unsigned long entityKey = 0 );

private:

    TA_Base_Core::StateUpdateMessageSender*     m_stateUpdateSender;
    TA_Base_Core::StateUpdateMessageSender*     m_stateUpdateRequestSender;

    //TA_Base_Core::ReEntrantThreadLockable       m_lock;
    ItaEcsCachedConfig&                         m_ecsCachedConfigInstance;
};



#endif // !defined(CtaEcsSynchronisation_INCLUDED)





