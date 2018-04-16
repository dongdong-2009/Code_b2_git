/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/AtsWatchDogTable.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * This represents the ATS Train Table
  */
#ifndef ATSWATCHDOGTABLE_H
#define ATSWATCHDOGTABLE_H

#include <sstream>

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "app/ats/AtsAgent/src/DataManager.h"
#include "app/ats/AtsAgent/src/RawTable.h"

namespace TA_IRS_App
{
    class AtsWatchDogTable
    {
    public:

		/**
		  * Constructor
		  *
		  */
		AtsWatchDogTable(unsigned long entityKey, 
						 unsigned long locationKey, 
						 TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
						 unsigned long atsTablesBaseAddress,
						 DataManager& dataManager);

		virtual ~AtsWatchDogTable();

		static std::string getStaticType()
		{
			return "AtsWatchDogTable";
		};

		static unsigned short getStaticModbusRecordSize(TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType);

		unsigned int	getNumberOfAtsRecords();
		unsigned short	getAtsModbusStartAddress();
		unsigned short	getAtsModbusEndAddress();
		unsigned short	getAtsModbusSize();
		void			importAtsTable(RawTable& rawTable, int connection);

	protected:
		
		bool checkAttribute(std::string attribute);
	
    private:
        // Disable default constructor, copy constructor and assignment operator
		AtsWatchDogTable();
        AtsWatchDogTable( const AtsWatchDogTable& atsPlatformRecord);
        AtsWatchDogTable& operator=(const AtsWatchDogTable &);

	protected:

		unsigned long						m_entityKey;
		unsigned long						m_locationKey;

		/** which location this table is catering for */
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType	
											m_locationType;

		/** Base address for all the ATS tables */
		int									m_atsTablesBaseAddress;

		DataManager&						m_dataManager;
    };
}

#endif
