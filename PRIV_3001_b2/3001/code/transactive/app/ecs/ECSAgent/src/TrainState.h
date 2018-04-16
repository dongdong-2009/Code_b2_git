/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/TrainState.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#if !defined(TrainState__INCLUDED_)
#define TrainState__INCLUDED_

#include "bus/ecs/ecs_agent/IDL/src/CommonEcsAgentCorbaDef.h"



struct TrainState
{
    TrainState(int trainId=TA_IRS_Bus::ECS_NONE_OR_INVALID)
     :
	   m_trainId(trainId),  
       m_isOa1Valid(false),
	   m_isLocalisationValid(false),
       m_isInEmergency(false),
       m_isHotCongested(false),
       m_isStalledCongested(false),
       m_isHot(false),
       m_isStalled(false),
	   m_isIgnoringCongestion(false)
    { }

    bool getEmergencyDataPointState()
    {
        return m_isInEmergency;
    }

    unsigned long getCongestionDataPointState()
    {
		// marvin++ added support for train ignoring congestion
		if(!m_isIgnoringCongestion)
		{
			if (m_isHotCongested)
			{
				return TA_IRS_Bus::ECS_ZONE_CONGESTION_ENUM_HITEMP_CONGESTION;
			}
			else if (m_isStalledCongested)
			{   
				return TA_IRS_Bus::ECS_ZONE_CONGESTION_ENUM_STALLED_CONGESTION;
			}
			else if (m_isHot)
			{   
				return TA_IRS_Bus::ECS_ZONE_CONGESTION_ENUM_HITEMP;
			}
			else if (m_isStalled)
			{   
				return TA_IRS_Bus::ECS_ZONE_CONGESTION_ENUM_STALLED;
			}
		}

        return TA_IRS_Bus::ECS_ZONE_CONGESTION_ENUM_NORMAL;
    }

    int  m_trainId;
    bool m_isOa1Valid;
	bool m_isLocalisationValid;
    bool m_isInEmergency;
    bool m_isHotCongested;
    bool m_isStalledCongested;
    bool m_isHot;
    bool m_isStalled;
	bool m_isIgnoringCongestion;
};


#endif // !defined(TrainState__INCLUDED_)
