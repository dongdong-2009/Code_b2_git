/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/EcsGlobals.h $
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

#if !defined(EcsGlobals_INCLUDED)
#define EcsGlobals_INCLUDED

#include <iostream>
#include <string>
#include <set>
#include <algorithm>
#include <vector>
#include <list>
#include "bus/ecs/ecs_agent/IDL/src/IStationEcsAgentCorbaDef.h"
#include "bus/ecs/ecs_agent/IDL/src/IMasterEcsAgentCorbaDef.h"
#include "bus/ecs/ecs_agent/IDL/src/CommonEcsAgentCorbaDef.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"



namespace TA_Base_Bus
{
    class DataPoint;
}

typedef std::vector<TA_Base_Bus::DataPoint*>                            DataPoints;
typedef DataPoints::iterator                                            DataPointsIt;

typedef std::set<TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode>     ProposedModeSet;
typedef ProposedModeSet::iterator                                       ProposedModeSetIt;


namespace TA_IRS_Bus
{
    static bool operator<(const TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode& lhs, const TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode& rhs)
    {
        if (lhs.modeType == rhs.modeType)
        {
            if (lhs.locationKey == rhs.locationKey)
            {
                return (lhs.mode < rhs.mode);
            }    
            else
            {
                return (lhs.locationKey < rhs.locationKey);
            }
        }
        else
        {
            return (lhs.modeType < rhs.modeType);
        }
    }
}

// We implement this class to do absolutely nothing.  This should be used by the DataPointProxies
// that only require write access 
class NullEventProcessor : public TA_Base_Bus::IEntityUpdateEventProcessor
{
    public:
		virtual void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
        { 
            // do nothing
        }
};


#if defined(WIN32) && defined(_CONSOLE) && defined(_DEBUG)
    #define LOGOUT(x)                   \
    {                                   \
        std::cout << x << std::endl;    \
    }                                   
#else
    #define LOGOUT(x)
#endif

struct MasterModePair
{
    MasterModePair()
    : first(TA_IRS_Bus::ECS_NONE_OR_INVALID), 
      second(TA_IRS_Bus::ECS_NONE_OR_INVALID)
    { }
    
    unsigned long first;
    unsigned long second;
};

const std::string   NUMERALS = "0123456789";


#endif // !defined(EcsGlobals_INCLUDED)
