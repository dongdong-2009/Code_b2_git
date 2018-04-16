/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/io_manager/src/IDataPointProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#ifndef SCADA_I_DATAPOINT_PROCESSOR_H_
#define SCADA_I_DATAPOINT_PROCESSOR_H_

#include <string>
#include "bus/scada/datapoint_library/src/DataPoint.h"

namespace TA_Base_Bus
{
	class DataPoint;
	class DataPointState;

	/** Interface for ProtocolUpdater submit datapoint updating data. 
	 * The derived processor will process those datas.
     *  
     */
	class IDataPointProcessor
	{
	public:
		virtual ~IDataPointProcessor(){};

		/**
         * submit the updating datas.
         *
         * @param dp: a pointer to DataPoint.
         * @param dpState: DataPointState reference.
         * @return void
         */
		virtual void submitData(DataPoint* dp, DataPointState& dpState) = 0;

	};

};

#endif
