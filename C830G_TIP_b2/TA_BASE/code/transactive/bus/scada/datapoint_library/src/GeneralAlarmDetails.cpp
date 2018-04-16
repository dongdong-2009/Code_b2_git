/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datapoint_library/src/GeneralAlarmDetails.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER

#include "bus/scada/datapoint_library/src/GeneralAlarmDetails.h"

#include "core/message/types/DataPointAlarms_MessageTypes.h"

namespace TA_Base_Bus
{
    GeneralAlarmDetails::GeneralAlarmDetails( TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess,
        unsigned long  dpParentEntityKey,
        unsigned long        dpEntityKey,
        unsigned long        dpEntityTypeKey,
        unsigned long	    regionKey,
        unsigned long	    subsystemKey,
        const std::string &  entityName,
        bool                 almEnabled,
        int                  almDelay,
        int                  almSeverity,
        const std::string &  almMessage,
        const std::string &	MMSConfiguration,
        const DpValue &      almValue,
        MmsScadaUtility &    mmsScadaUtility )
        :
    DataPointAlarmDetails( dataPointEntityAccess, TA_Base_Core::DataPointAlarms::DpGeneral, dpParentEntityKey, 
        dpEntityKey, dpEntityTypeKey, regionKey,
        subsystemKey, entityName, almEnabled, almDelay, almSeverity, 
        almMessage, MMSConfiguration,
        TA_Base_Bus::DptGeneral, almValue, mmsScadaUtility )
    {
        // Just pass parameters onto base class
        
        //setValue( almValue );
    }
    
    
    GeneralAlarmDetails::~GeneralAlarmDetails() 
    {
    }
    
    
    bool GeneralAlarmDetails::inAlarm( const DataPointState & state )
    {
        // if the value of the state to be test equals the
        // pre-defined alarm value
        if ( state.getValue() == getValue() )
        {
            return true;
        }
        
        return false;
    }
    

    std::string GeneralAlarmDetails::getAlarmTypeStr()
	{
		std::string alarmTypeStr = DPTALM_GENERAL_SHORT;		
		return alarmTypeStr;
	}

}
