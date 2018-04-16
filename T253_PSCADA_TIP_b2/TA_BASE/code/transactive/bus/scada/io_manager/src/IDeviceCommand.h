/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/io_manager/src/IDeviceCommand.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#ifndef SCADA_I_DEVICE_COMMAND_H_
#define SCADA_I_DEVICE_COMMAND_H_

#include <string>

namespace TA_Base_Bus
{

	/** Interface for sending command.
	 * all protocol device write commands should derive from this interface.
     *  
     */
	class IDeviceCommand
	{
	public:
		virtual ~IDeviceCommand(){};
		
		/**
         * get the command type name.
         *
         * @return std::string: command type name.
         */
		virtual std::string getStaticType() = 0;

	};

};

#endif
