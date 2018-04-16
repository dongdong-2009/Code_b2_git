/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datapoint_library/src/TRUEAlarmDetails.h $
  * @author  J. Welton
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2012/02/06 16:15:14 $
  * Last modified by : $Author: haijun.li $
  *
  *
  *	Represents an alarm condition for a boolean true value
  *
  *
  */

#ifndef TRUEALARMDETAILS_H
#define TRUEALARMDETAILS_H

#include "bus/scada/datapoint_library/src/DataPointAlarmDetails.h"
#include "bus/scada/datapoint_library/src/DataPointState.h"

namespace TA_Base_Bus
{
	class TRUEAlarmDetails : public DataPointAlarmDetails
	{
	public:

		TRUEAlarmDetails( TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess,
                          unsigned long		    dpParentEntityKey,
						  unsigned long		    dpEntityKey,
						  unsigned long		    dpEntityTypeKey,
						  unsigned long		    regionKey,
						  unsigned long		    subsystemKey,
						  const std::string &	entityName,
						  bool                  almEnabled,
						  int                   almDelay,
						  int                   almSeverity,
                          const std::string &	almMessage,
						  const std::string &	MMSConfiguration,
						  const DpValue &       almValue,
                          MmsScadaUtility&      mmsScadaUtility);
        
        virtual ~TRUEAlarmDetails();

        virtual std::string getAlarmTypeStr();
	
	protected:

		virtual bool inAlarm( const DataPointState & state );
	};
}

#endif	
