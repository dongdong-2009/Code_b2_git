/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/IscsWatchDogTable.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last mofified by:  $Author: builder $
  *
  * This represents the ISCS Train Table
  */
#ifndef ISCSWATCHDOGTABLE_H
#define ISCSWATCHDOGTABLE_H

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "app/ats/AtsAgent/src/RawTable.h"
#include "app/ats/AtsAgent/src/DataManager.h"

namespace TA_IRS_App
{
    class IscsWatchDogTable
    {
    public:

		/**
		  * Constructor
		  *
		  */
		IscsWatchDogTable(TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
						  int iscsTablesBaseAddress, DataManager& dataManager);

		virtual ~IscsWatchDogTable();

		static std::string getStaticType()
		{
			return "IscsWatchDogTable";
		};

		static unsigned short getStaticModbusRecordSize(TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType);

		unsigned int	getNumberOfIscsRecords();
		unsigned short	getIscsModbusStartAddress();
		unsigned short	getIscsModbusEndAddress();
		void			updateWatchdogCounter(int differenceInSecs);
		void			exportIscsTable(RawTable& rawTable);

	protected:
		unsigned short	getIscsModbusSize();
	
	protected:
		
		bool checkAttribute(std::string attribute);

    private:
        // Disable default constructor, copy constructor and assignment operator
        IscsWatchDogTable();
		IscsWatchDogTable( const IscsWatchDogTable& iscsWatchDogTable);
        IscsWatchDogTable& operator=(const IscsWatchDogTable &);

	protected:

		unsigned short						m_chronologicalCounter;

		/** which location this table is catering for */
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType	
											m_locationType;

		/** Base address for all the ATS tables */
		int									m_iscsTablesBaseAddress;

		DataManager&						m_dataManager;
    };
}

#endif
