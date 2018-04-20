/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datapoint_library/src/CombinedOutputDataPoint.cpp $
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

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER

#include "bus/scada/datapoint_library/src/CombinedOutputDataPoint.h"


namespace TA_Base_Bus
{
	CombinedOutputDataPoint::CombinedOutputDataPoint( TA_Base_Core::DataPointEntityDataPtr					dataPointEntityAccess,
													  TA_Base_Bus::EDataPointType			                dpType,
													  TA_Base_Bus::EDataPointDataType               		dpDataType,
                                                      ScadaPersistencePtr                                   persistence,
                                                      TA_Base_Core::ScadaRootEntityData&                    scadaRootEntityData,
                                                      MmsScadaUtility&                                      mmsScadaUtility,
                                                      CommandProcessor&                                     commandProcessor )
	:
	SafetyOutputDataPoint ( dataPointEntityAccess, dpType, dpDataType, persistence, scadaRootEntityData, mmsScadaUtility, commandProcessor )
	{
		// do nothing
	}


	CombinedOutputDataPoint::~CombinedOutputDataPoint()
	{
		// do nothing
	}
}
