/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datapoint_library/src/DataPointAlarmDetailsFactory.h $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * Class for creating and modifying the various types
  * of DataPointAlarmDetails.
  *
  */

#ifndef DATAPOINTALARMDETAILSFACTORY_H
#define DATAPOINTALARMDETAILSFACTORY_H

#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

namespace TA_Base_Bus
{
    // forward declaration
	class DpValue;
	class DataPointState;
	class DataPointAlarmDetails;
    class MmsScadaUtility;

	class DataPointAlarmDetailsFactory
	{
	public:
        DataPointAlarmDetailsFactory() {};
		~DataPointAlarmDetailsFactory() {};

		DataPointAlarmDetails * createAlarmDetails( TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess,
                                                    const TA_Base_Bus::EDataPointAlarms & almType,
													bool almEnabled,
													int almDelay,
													int almSeverity,
													const std::string & almMessage,
													const DpValue & almValue,
													const std::string & entityName,
													const std::string &	MMSConfiguration,
													unsigned long dpParentEntityKey,
													unsigned long entityKey,
													unsigned long entityTypeKey,
													unsigned long regionKey,
													unsigned long subsystemKey,
													double alarmThresholdConstant,
                                                    MmsScadaUtility& mmsScadaUtility
													);

        void updateAlarmDetails( const std::string OMFlag,
								 DataPointAlarmDetails * alm,
								 bool almEnabled,
								 int almDelay,
								 int almSeverity,
								 const std::string & almMessage,
								 const std::string & MMSConfiguration,
								 const DpValue & almValue,
								 const DataPointState & almState,
								 bool alarmInhibited,
								 double alarmThresholdConstant
                                 );
	};
}

#endif
