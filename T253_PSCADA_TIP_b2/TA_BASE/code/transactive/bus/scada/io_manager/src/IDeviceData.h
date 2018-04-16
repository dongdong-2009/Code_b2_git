/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/io_manager/src/IDeviceData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#ifndef SCADA_I_DEVICE_DATA_H_
#define SCADA_I_DEVICE_DATA_H_

#include <string>

namespace TA_Base_Bus
{

	/** Interface for all kinds of device data types.
     *  
     */
	class IDeviceData
	{
	public:
		virtual ~IDeviceData(){};

		/**
         * get the device data type name.
         *
         * @return std::string: device data type name.
         */
		virtual std::string getStaticType() = 0;

	};

};

#endif
