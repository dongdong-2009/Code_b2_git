/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/IModbusAgentEntityData.h $
  * @author Dhanshri
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by: $Author: grace.koh $
  * 
  * IModbusAgentEntityData provides read-only access to ModbusAgent entity data.
  */

#ifndef IModbusAgent_ENTITY_DATA_H
#define IModbusAgent_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{

    class IModbusAgentEntityData : public virtual IEntityData
    {
    public:

        /**
         * getIntervalBetweenModbusTransaction
         *
         * Returns the IntervalBetweenModbusTransaction in msecs for ModbusAgent entity
         *
         * @return The integer value presenting the time in msecs
         */
        virtual  const int getIntervalBetweenModbusTransaction() = 0;

        /**
         * getStationName
         *
         * Returns the name of the Station the ModbusAgent entity is configured to monitor
         *
         * @return The name of the Station the ModbusAgent entity is configured to monitor
         */
        virtual const std::string getStationName() = 0;

       // TD16169 ++
		/**
         * getAssetName
         *
         * Returns the asset name of the ModbusAgent entity is configured to monitor
         *
         * @return The asset name of the ModbusAgent entity is configured to monitor
         */
		virtual  const std::string getAssetName() = 0;

        /**
         * getMaxModbusLength
         *
         * Returns the getMaxModbusLength for the ModbusAgent entity 
         *
         */
		virtual  const int getMaxModbusLength() = 0;

        /**
         * getOverlapModbusRequest
         *
         * Returns true if the OverlapModbusRequest of RTU is set yes in ModbusAgent,
		 * false ortherwise
         *
         */
		virtual  const bool getOverlapModbusRequest() = 0;

		/**
         * getStandardModbusLengthLimit
         *
         * Returns true if the StandardModbusLengthLimit of RTU is set yes in ModbusAgent,
		 * false ortherwise
         *
         */
		virtual  const bool getStandardModbusLengthLimit() = 0;

        virtual  const int getPollTimeout() = 0;

    };

} //close namespace TA_Base_Core

#endif

