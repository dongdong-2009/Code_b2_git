/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/ElectricalSubsectionTable.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * This represents the ATS ElectricalSubsection Table
  */
#ifndef ELECTRICALSUBSECTIONTABLE_H
#define ELECTRICALSUBSECTIONTABLE_H

#include <map>

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"

#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"

#include "app/ats/AtsAgent/src/IIscsTable.h"
#include "app/ats/AtsAgent/src/DataManager.h"

namespace TA_IRS_App
{
    class ElectricalSubsectionTable : public IIscsTable
    {
    public:
		/**
		  * Constructor
		  *
		  */
		ElectricalSubsectionTable(unsigned long locationKey, TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
								  int iscsTablesBaseAddress, DataManager& dataManager);

		static std::string getStaticType()
		{
			return "ElectricalSubsectionTable";
		};

		//
		// IIscsTable Implementation
		//
		unsigned int	getNumberOfIscsRecords();
		unsigned short	getIscsModbusStartAddress();
		unsigned short	getIscsModbusEndAddress();
		void			exportIscsTable(RawTable& rawTable);
		bool			tableWriteAllowed();

	protected:	
		void exportRecord (RawTable& rawTable, unsigned int recordByteOffset,
						   unsigned short subsectionId);

		unsigned short	getIscsModbusRecordSize();
		unsigned short	getIscsModbusSize();

    private:
        // Disable default constructor, copy constructor and assignment operator
        ElectricalSubsectionTable();
		ElectricalSubsectionTable( const ElectricalSubsectionTable& iscsElectricalSubsectionTable);
        ElectricalSubsectionTable& operator=(const ElectricalSubsectionTable &);

	protected:

        /** which location this table is catering for */
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType	
											m_locationType;

		/** Base address for all the ATS tables */
		int									m_iscsTablesBaseAddress;

		DataManager&						m_dataManager;
    };
}

#endif 
