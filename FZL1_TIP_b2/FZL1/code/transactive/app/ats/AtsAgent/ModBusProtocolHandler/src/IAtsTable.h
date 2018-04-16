/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/ModBusProtocolHandler/src/IAtsTable.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This represents a generic ATS Table
  */
#ifndef IATSTABLE_H
#define IATSTABLE_H

#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"

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
		virtual void parseAtsTable(TA_IRS_Bus::RawTable& rawTable) = 0;


    };
}

#endif // !defined(ATSPLATFORMTABLE_H)
