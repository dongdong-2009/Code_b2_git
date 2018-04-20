/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datapoint_library/src/CombinedOutputDataPoint.h $
  * @author  HoaVu
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  *
  *	A Safety Output Datapoint maps a single value to several target
  * datapoints.  Combined Output datapoint is controlled by setting a state.
  * The associated output datapoints are set according to a state table.
  *
  */

#ifndef COMBINED_OUTPUT_DATAPOINT_H
#define COMBINED_OUTPUT_DATAPOINT_H

#include "bus/scada/datapoint_library/src/SafetyOutputDataPoint.h"

namespace TA_Base_Bus
{

	class CombinedOutputDataPoint : public SafetyOutputDataPoint
	{
	public:


		CombinedOutputDataPoint ( TA_Base_Core::DataPointEntityDataPtr				dataPointEntityAccess,
								  TA_Base_Bus::EDataPointType		                dpType,
					   			  TA_Base_Bus::EDataPointDataType	                dpDataType,
                                  ScadaPersistencePtr                               persistence,
                                  TA_Base_Core::ScadaRootEntityData&                scadaRootEntityData,
                                  MmsScadaUtility&                                  mmsScadaUtility,
                                  CommandProcessor&                                 commandProcessor);


		virtual ~CombinedOutputDataPoint();
	};

}

#endif

