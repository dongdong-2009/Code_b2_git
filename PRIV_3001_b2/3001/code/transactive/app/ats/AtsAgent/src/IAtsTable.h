/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/IAtsTable.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This represents a generic ATS Table
  */
#ifndef IATSTABLE_H
#define IATSTABLE_H

#include <map>

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"

#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"

#include "app/ats/AtsAgent/src/RawTable.h"

namespace TA_Bus
{
    class DataPoint;
}

namespace TA_IRS_App
{
    class IAtsTable
    {
    public:

        /** Imports the table data from raw table as read by the Modbus
         * connection
         *
         * @param rawTable the raw table that the Modbus connection has
         *				   populated
         */
        virtual void				importAtsTable(RawTable& rawTable) = 0;

        /** Accessor for the number of records in the table. Location specific.
         * 
         * @return the number of records in this table at its specified location
         */
        virtual unsigned int		getNumberOfAtsRecords() = 0;


        /** Accessor for the Start Address of the data for this table at the Ats
         * 
         * @return the address of this table
         */
        virtual unsigned short		getAtsModbusStartAddress() = 0;


        /** Accessor for the Start Address of the data for this table at the Ats
         * 
         * @return the address of this table
         */
        virtual unsigned short		getAtsModbusEndAddress() = 0;

		/** Is the table to be read */
		virtual bool tableReadAllowed() = 0;
    };
}

#endif // !defined(ATSPLATFORMTABLE_H)
