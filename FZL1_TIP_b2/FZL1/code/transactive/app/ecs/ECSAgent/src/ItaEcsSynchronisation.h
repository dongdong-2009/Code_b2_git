/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ECSAgent/src/ItaEcsSynchronisation.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// CachedConfig.h: interface for the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(ItaEcsSynchronisation_INCLUDED)
#define ItaEcsSynchronisation_INCLUDED

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/ecs/ecs_agent/IDL/src/CommonEcsAgentCorbaDef.h"

class ItaEcsSynchronisation
{

public:

    virtual void sendEcsAgentStateUpdateRequest()=0;

    virtual void sendEcsAgentParameterUpdate( TA_IRS_Bus::EEcsAgentParameter parameter, 
                                      const CORBA::Any& parameterData,
                                      unsigned long entityKey = 0 )=0;
};



#endif // !defined(ItaEcsSynchronisation_INCLUDED)





