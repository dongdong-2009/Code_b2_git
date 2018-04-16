/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/alarm/alarm_common/src/AlarmConstants.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2017/10/31 16:21:54 $
 * Last modified by:  $Author: CM $
 *
 * Constants sharte by the Alarm Agent and libraries.
 */

#if !defined(ALARMCONSTANTS_H_INCLUDED)
#define ALARMCONSTANTS_H_INCLUDED

#include <string>

namespace TA_Base_Bus
{
	namespace AlarmConstants
	{
		static const std::string OPERATOR_DELIMITER="/";

		static const std::string RPARAM_MILLISECOND = "Millisecond";

		static const std::string RPARAM_USESOURCETIME = "UseSourceTime";

		static const std::string RPARAM_NOGRIDLINES = "NoGridLines";

		static const std::string RPARAM_TIMESTAMPCOLWIDTH = "TimestampColWidth";

		static const unsigned long DEFAULT_TIMESTAMPCOLWIDTH = 165; //TD15005

	};
};

#endif // !defined(ALARM_CONSTANTS_H_INCLUDED)
