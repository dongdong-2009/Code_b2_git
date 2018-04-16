/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/alarm/alarm_list_control/src/Utilities.cpp $
 * @author:  Anita Lee
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * This class provides all the information for the decision support column.
 */

#include "bus/alarm/alarm_list_control/src/Utilities.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Bus
{

    Utilities* Utilities::s_utilities = NULL;
    TA_Base_Core::NonReEntrantThreadLockable Utilities::s_singletonLock;

    
    Utilities& Utilities::getInstance()
    {
        // protect the instance check / creation
        TA_Base_Core::ThreadGuard guard(s_singletonLock);

        if(s_utilities == 0)
        {
            s_utilities = new Utilities();
        }
        
        return *s_utilities;
    }

    
    std::string Utilities::getDSSDisplayString(const TA_Base_Core::AlarmDetailCorbaDef* alarm)
    {
        switch(alarm->decisionSupportState)
        {
            case(TA_Base_Core::RelatedPlanHasBeenRun):
                return "R";

            case(TA_Base_Core::RelatedPlanHasBeenPostponed):
                return "X";

            case(TA_Base_Core::RelatedAutomaticPlanNotRun):
                return "A";

            case(TA_Base_Core::RelatedSemiAutomaticPlanNotRun):
                return "S";

            default:
                return "-";
        }
    }


    std::string Utilities::getMMSDisplayString(const TA_Base_Core::AlarmDetailCorbaDef* alarm)
    {
        switch (alarm->mmsState)
		{
			case TA_Base_Core::MMS_NONE:
				return "";

			case TA_Base_Core::MMS_SEMI:
				return "S";

			case TA_Base_Core::MMS_AUTO:
				return "A";

			case TA_Base_Core::MMS_JR_SENT:
				return "R";

			case TA_Base_Core::MMS_ALM_SENT:
				TA_ASSERT(false, "Received MMS_ALM_SENT mmsState, which is not supported");

			case TA_Base_Core::MMS_INHIBIT:
				return "I";

			case TA_Base_Core::MMS_FAILED:
				return "D";

			default:
				TA_ASSERT(false, "Received unknown mmsState. Has the MmsStateType enum changed?");
				return "";
        }
    }


    std::string Utilities::getAvalancheDisplayString(const TA_Base_Core::AlarmDetailCorbaDef* alarm)
    {
        if (alarm->isHeadOfAvalanche)
        {
            return "H";
        }
		else if (alarm->isChildOfAvalanche)
		{
			return "F";
		}

        return "";
    }


} //End of TA_Base_Bus

