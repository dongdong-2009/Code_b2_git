/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsFixedBlockMapAccessFactory.cpp $
  * @author:  San Teo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2013/04/01 16:13:40 $
  * Last modified by:  $Author: raghu.babu $
  * 
  * AtsFixedBlockMapAccessFactory is a singleton that is used to retrieve AtsFixedBlockMap objects either from the
  * database or newly created. All AtsFixedBlockMap objects returned will adhear to the IAtsFixedBlockMap interface.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/ats/src/AtsFixedBlockMapAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsFixedBlockMap.h"
#include "core/data_access_interface/ats/src/AtsFbMapRecord.h"
#include "core/data_access_interface/ats/src/ConfigAtsFbMapRecord.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include <sstream>
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_Core
{
    const std::string AtsFixedBlockMapAccessFactory::CctvZoneType = "VI_ZONES";
    const std::string AtsFixedBlockMapAccessFactory::RadioZoneType = "IAGO_ZONES";
    const std::string AtsFixedBlockMapAccessFactory::EcsZoneType = "EC_ZONES";
	static const std::string KEY_COL = "ATFBLO_ID";
    static const std::string BRANCHID_COL = "BRANCH_ID";
	static const std::string TRACK_COL = "TRACK";
    static const std::string MINABSCISSA_COL = "MIN_ABSCISSA";
    static const std::string MAXABSCISSA_COL = "MAX_ABSCISSA";
    static const std::string GENERICID_COL = "GENERIC_ID";
	static const std::string ZONETYPE_COL = "ZONE_TYPE";
	static const std::string ZONETYPENAME_COL = "ZONE_TYPE_NAME";
	static const std::string ARM_ID_COL = "ARM_ID";
	static const std::string BRANCHNAME_COL = "BRANCH_NAME";


    AtsFixedBlockMapAccessFactory* AtsFixedBlockMapAccessFactory::m_instance = 0;

    AtsFixedBlockMapAccessFactory& AtsFixedBlockMapAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new AtsFixedBlockMapAccessFactory();
        }
        return *m_instance;
    }


	IAtsFbMapRecords AtsFixedBlockMapAccessFactory::getAllAtsFbMapRecords( const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create the SQL string to retrieve the name of the AtsFixedBlockMap
        // this also doubles as a check to ensure an AtsFixedBlockMap with the specified
        // PKEY actually exists.
        /*std::ostringstream sql;
		sql << "SELECT " << KEY_COL << ", " << BRANCHID_COL << ", " << TRACK_COL << ", ";
		sql << MINABSCISSA_COL << ", " << MAXABSCISSA_COL << ", " << GENERICID_COL << ", ";
		sql << ZONETYPE_COL << ", " << ZONETYPENAME_COL << " FROM AT_FIXED_BLOCK_MAP_V";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_FIXED_BLOCK_MAP_STD_SELECT_47503);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_FIXED_BLOCK_MAP_SELECT_47503);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_FIXED_BLOCK_MAP_SELECT_47503);

        std::vector<std::string> columnNames;
		columnNames.push_back(KEY_COL);
        columnNames.push_back(BRANCHID_COL);
		columnNames.push_back(TRACK_COL);
        columnNames.push_back(MINABSCISSA_COL);
        columnNames.push_back(MAXABSCISSA_COL);
        columnNames.push_back(GENERICID_COL);
		columnNames.push_back(ZONETYPE_COL);
        columnNames.push_back(ZONETYPENAME_COL);
		columnNames.push_back(ARM_ID_COL);
		columnNames.push_back(BRANCHNAME_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IAtsFbMapRecords AtsFbMapRecordPointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long id = data->getUnsignedLongData(i,KEY_COL);
                if (id != 0)
                {
                    if (readWrite)
                    {
                        AtsFbMapRecordPointerVector.push_back( new ConfigAtsFbMapRecord(i, *data));
                    }
                    else
                    {
                        AtsFbMapRecordPointerVector.push_back( new AtsFbMapRecord(i, *data));
                    }
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return AtsFbMapRecordPointerVector;
    }


	IAtsFbMapRecords AtsFixedBlockMapAccessFactory::getAllAtsFbMapRecordsForZoneType( const std::string& zoneType, const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create the SQL string to retrieve the name of the AtsFixedBlockMap
        // this also doubles as a check to ensure an AtsFixedBlockMap with the specified
        // PKEY actually exists.
        /*std::ostringstream sql;
		sql << "SELECT " << KEY_COL << ", " << BRANCHID_COL << ", " << TRACK_COL << ", ";
		sql << MINABSCISSA_COL << ", " << MAXABSCISSA_COL << ", " << GENERICID_COL<< ", ";
		sql << ZONETYPE_COL << ", " << ZONETYPENAME_COL << " FROM AT_FIXED_BLOCK_MAP_V WHERE ";
		sql << ZONETYPE_COL << " = (SELECT PKEY FROM ZONETYPE WHERE NAME = '" << zoneType << "')";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_FIXED_BLOCK_MAP_STD_SELECT_47504, zoneType);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_FIXED_BLOCK_MAP_SELECT_47504, zoneType);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_FIXED_BLOCK_MAP_SELECT_47504, zoneType);

        std::vector<std::string> columnNames;
		columnNames.push_back(KEY_COL);
        columnNames.push_back(BRANCHID_COL);
		columnNames.push_back(TRACK_COL);
        columnNames.push_back(MINABSCISSA_COL);
        columnNames.push_back(MAXABSCISSA_COL);
        columnNames.push_back(GENERICID_COL);
		columnNames.push_back(ZONETYPE_COL);
		columnNames.push_back(ZONETYPENAME_COL);
		columnNames.push_back(ARM_ID_COL);
		columnNames.push_back(BRANCHNAME_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IAtsFbMapRecords AtsFbMapRecordPointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long id = data->getUnsignedLongData(i,KEY_COL);
                if (id != 0)
                {
                    if (readWrite)
                    {
                        AtsFbMapRecordPointerVector.push_back( new ConfigAtsFbMapRecord(i, *data));
                    }
                    else
                    {
                        AtsFbMapRecordPointerVector.push_back( new AtsFbMapRecord(i, *data));
                    }
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return AtsFbMapRecordPointerVector;
    }

    IAtsFixedBlockMap* AtsFixedBlockMapAccessFactory::getAtsFixedBlockMap(const std::string zoneType)
    {
        TA_IRS_Core::IAtsFbMapRecords fbMapRecords = getAllAtsFbMapRecordsForZoneType(zoneType, false);
		TA_IRS_Core::IAtsFbMapRecords::iterator it = fbMapRecords.begin();

        TA_IRS_Core::BranchZoneMap branchZoneMap;
        TA_IRS_Core::IAtsFbMapRecords branchDetailVector;
		unsigned long branchId;

		if (it != fbMapRecords.end())
		{
			do
			{
				// get the first row
				branchId = (*it)->getBranchId();

				for (; it != fbMapRecords.end(); it++ )
				{
					unsigned long rowBranchId = (*it)->getBranchId();

					//
					// if this row's branchid is new, save the old AtsFbMapRecordVector into the map
					if (rowBranchId != branchId)
					{
						// if it is a different branch, then add the branch vector that we've previously created into the map
						branchZoneMap.insert(BranchZoneMap::value_type(branchId, branchDetailVector));
						branchId = rowBranchId; // set it to the new Id
						branchDetailVector.erase(branchDetailVector.begin(), branchDetailVector.end());
					}

					// add the branch details to the vector
					branchDetailVector.push_back(*it);
				}
			}
			while( it != fbMapRecords.end() );

			// add the last branch vector to the map
			branchZoneMap.insert(BranchZoneMap::value_type(branchId, branchDetailVector));
		}

        // Return the AtsFixedBlockMap pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return new AtsFixedBlockMap(branchZoneMap);
    }

	IConfigAtsFbMapRecord* AtsFixedBlockMapAccessFactory::createAtsFbMapRecord()
    {            
        return new ConfigAtsFbMapRecord();    
    }


    IConfigAtsFbMapRecord* AtsFixedBlockMapAccessFactory::copyAtsFbMapRecord(const IConfigAtsFbMapRecord* atsFbMapRecordToCopy)
    {
        FUNCTION_ENTRY("copyAtsBranchTrack");

        TA_ASSERT(atsFbMapRecordToCopy !=  NULL, "The AtsBranchTrack to copy was NULL");
        
        const ConfigAtsFbMapRecord* cast = dynamic_cast<const ConfigAtsFbMapRecord*>(atsFbMapRecordToCopy);

        TA_ASSERT(cast != NULL, "AtsBranchTrack passed could not be converted into a ConfigAtsBranchTrack");

        FUNCTION_EXIT;
        return new ConfigAtsFbMapRecord(*cast);

    }

} // closes TA_Core


