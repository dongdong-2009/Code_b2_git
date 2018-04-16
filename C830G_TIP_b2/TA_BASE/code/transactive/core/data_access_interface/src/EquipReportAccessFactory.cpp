/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/EquipReportAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */
#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "trucated to 255 character" warning
#endif

#include "EquipReportAccessFactory.h"

namespace TA_Base_Core
{
	EquipReportAccessFactory* EquipReportAccessFactory::m_instance = 0;

	EquipReportAccessFactory& EquipReportAccessFactory::getInstance()
	{
        if( m_instance == 0 )
        {
            m_instance = new EquipReportAccessFactory();
        }
        return *m_instance;
	}


	void EquipReportAccessFactory::getEquipReportBySql(const std::string& sql, std::vector<IEquipReport*>& reports, const bool readWrite)
	{
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		columnNames.push_back("SCEREP_ID");
        columnNames.push_back("REPORT_NAME");
        columnNames.push_back("LOCATION_ID");
		columnNames.push_back("PROFILE");
		columnNames.push_back("REPORT_TYPE_FILTER_FLAG");
		columnNames.push_back("REPORT_TYPE_FILTER");
		columnNames.push_back("ASSET_FILTER_FLAG");
		columnNames.push_back("ASSET_FILTER");
		columnNames.push_back("DESC_FILTER_FLAG");
		columnNames.push_back("DESC_FILTER");
		columnNames.push_back("DPNAME_FILTER_FLAG");
		columnNames.push_back("DPNAME_FILTER");
		columnNames.push_back("DPTYPE_FILTER_FLAG");
		columnNames.push_back("DPTYPE_FILTER");
		columnNames.push_back("DPVALUE_FILTER_FLAG");
		columnNames.push_back("DPVALUE_FILTER");
		columnNames.push_back("STATUS_FILTER_FLAG");
		columnNames.push_back("SCAN_INHIBIT_FLAG");
		columnNames.push_back("ALARM_INHIBIT_FLAG");
		columnNames.push_back("CONTROL_INHIBIT_FLAG");
		columnNames.push_back("MMS_INHIBIT_FLAG");
		columnNames.push_back("TAGGED_FLAG");
		columnNames.push_back("TAGGED_PTW_FLAG");
		columnNames.push_back("OVERRIDDEN_FLAG");
		columnNames.push_back("STATUS_CHANGE_BY_FILTER_FLAG");
		columnNames.push_back("STATUS_CHANGE_BY_FILTER");
		columnNames.push_back("TIMESTAMP_FILTER_FLAG");
		columnNames.push_back("TIMESTAMP_FILTER");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
        IData* data = databaseConnection->executeQuery( sql, columnNames );

        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    // Create a shiny new equipment report that has all the information pre-loaded!
                    IEquipReport* report = NULL;

					// Assign the data as appropriate to the member variables.
					// These calls can throw DataExceptions, as documented in the comment of this method.
                    if ( readWrite )
                    {
                        if ( "0" == data->getStringData( i, columnNames[0] ) )
                        {
                            // if we have the equipment report where the key is 0 then we are not interested in it
                            // The user cannot configure this particular equipment report
                            continue;
                        }

						reports.push_back( new ConfigEquipReport(i, *data) );
                    }
                    else
                    {
						reports.push_back( new EquipReport(i, *data) );
                    }
                }
                catch (TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }
			delete data;
			data = NULL;
        }
        while (databaseConnection->moreData(data));
	}
	
	
	std::vector<IEquipReport*> EquipReportAccessFactory::getAllEquipReports(const bool readWrite)
	{
		FUNCTION_ENTRY("getAllEquipReports");

		// create the SQL string to retrieve all Equipment Reports for ALL locations
        std::ostringstream sql;
        sql << " select SCEREP_ID, REPORT_NAME, LOCATION_ID, PROFILE,"
			<< " REPORT_TYPE_FILTER_FLAG, REPORT_TYPE_FILTER,"
			<< " ASSET_FILTER_FLAG, ASSET_FILTER, DESC_FILTER_FLAG, DESC_FILTER,"
			<< " DPNAME_FILTER_FLAG, DPNAME_FILTER, DPTYPE_FILTER_FLAG, DPTYPE_FILTER,"
			<< " DPVALUE_FILTER_FLAG, DPVALUE_FILTER, STATUS_FILTER_FLAG,"
			<< " SCAN_INHIBIT_FLAG, ALARM_INHIBIT_FLAG, CONTROL_INHIBIT_FLAG, MMS_INHIBIT_FLAG,"
			<< " TAGGED_FLAG, TAGGED_PTW_FLAG, OVERRIDDEN_FLAG, "
			<< " STATUS_CHANGE_BY_FILTER_FLAG, STATUS_CHANGE_BY_FILTER,"
			<< " TIMESTAMP_FILTER_FLAG, TIMESTAMP_FILTER"
			<< " from SC_EQUIP_REPORT order by REPORT_NAME";

		std::vector<IEquipReport*> reports;
		getEquipReportBySql(sql.str(), reports, readWrite);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return reports;
	}
	
	
	std::vector<IEquipReport* >EquipReportAccessFactory::getAllEquipReportsByLocationKey(unsigned long aLocationKey, const bool readWrite)
	{
		FUNCTION_ENTRY("getAllEquipReportsByLocationKey");

		// create the SQL string to retrieve all Equipment Reports for ALL locations
        std::ostringstream sql;
        sql << " select SCEREP_ID, REPORT_NAME, LOCATION_ID, PROFILE,"
			<< " REPORT_TYPE_FILTER_FLAG, REPORT_TYPE_FILTER,"
			<< " ASSET_FILTER_FLAG, ASSET_FILTER, DESC_FILTER_FLAG, DESC_FILTER,"
			<< " DPNAME_FILTER_FLAG, DPNAME_FILTER, DPTYPE_FILTER_FLAG, DPTYPE_FILTER,"
			<< " DPVALUE_FILTER_FLAG, DPVALUE_FILTER, STATUS_FILTER_FLAG,"
			<< " SCAN_INHIBIT_FLAG, ALARM_INHIBIT_FLAG, CONTROL_INHIBIT_FLAG, MMS_INHIBIT_FLAG,"
			<< " TAGGED_FLAG, TAGGED_PTW_FLAG, OVERRIDDEN_FLAG, "
			<< " STATUS_CHANGE_BY_FILTER_FLAG, STATUS_CHANGE_BY_FILTER,"
			<< " TIMESTAMP_FILTER_FLAG, TIMESTAMP_FILTER"
			<< " from SC_EQUIP_REPORT where LOCATION_ID = " << aLocationKey
			<< " order by REPORT_NAME";

		std::vector<IEquipReport*> reports;
		getEquipReportBySql(sql.str(), reports, readWrite);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return reports;

	}

    IEquipReport *EquipReportAccessFactory::getEquipReportByKey(std::string& aEquipReportKey,  const bool readWrite)
	{
		FUNCTION_ENTRY("getEquipReportByKey");

		// create the SQL string to retrieve all Equipment Reports for ALL locations
        std::ostringstream sql;
        sql << " select SCEREP_ID, REPORT_NAME, LOCATION_ID, PROFILE,"
			<< " REPORT_TYPE_FILTER_FLAG, REPORT_TYPE_FILTER,"
			<< " ASSET_FILTER_FLAG, ASSET_FILTER, DESC_FILTER_FLAG, DESC_FILTER,"
			<< " DPNAME_FILTER_FLAG, DPNAME_FILTER, DPTYPE_FILTER_FLAG, DPTYPE_FILTER,"
			<< " DPVALUE_FILTER_FLAG, DPVALUE_FILTER, STATUS_FILTER_FLAG,"
			<< " SCAN_INHIBIT_FLAG, ALARM_INHIBIT_FLAG, CONTROL_INHIBIT_FLAG, MMS_INHIBIT_FLAG,"
			<< " TAGGED_FLAG, TAGGED_PTW_FLAG, OVERRIDDEN_FLAG, "
			<< " STATUS_CHANGE_BY_FILTER_FLAG, STATUS_CHANGE_BY_FILTER,"
			<< " TIMESTAMP_FILTER_FLAG, TIMESTAMP_FILTER"
			<< " from SC_EQUIP_REPORT where SCEREP_ID = '" << aEquipReportKey << "'"
			<< " order by REPORT_NAME";

		std::vector<IEquipReport*> reports;
		getEquipReportBySql(sql.str(), reports, readWrite);

        if ( 0 == reports.size())
        {
            std::ostringstream message;
			message << "No data found for EquipReport with key " << aEquipReportKey;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == reports.size(), "EquipmentReport key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return reports[0];	
	}


	IConfigEquipReport* EquipReportAccessFactory::createEquipReport()
	{
		return new ConfigEquipReport(); 
	}

	IConfigEquipReport* EquipReportAccessFactory::copyEquipReport(const IConfigEquipReport *aEquipReportToCopy, bool copyDependants /* = true*/)
	{
		const ConfigEquipReport* cast = dynamic_cast<const ConfigEquipReport*>(aEquipReportToCopy);

		return new ConfigEquipReport(*cast);
	}


	bool EquipReportAccessFactory::equipReportAlreadyExistsAtLocationByName(const unsigned long aLocationKey, const std::string &aEquipReportName)
	{
		// get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);

		// create the SQL string to retrieve all Equipment Reports with a specified reportname
        std::ostringstream sql;
        sql << "select SCEREP_ID FROM SC_EQUIP_REPORT ";
		sql << "where lower(REPORT_NAME) = lower('" << databaseConnection->escapeQueryString(aEquipReportName) << "') ";
		sql << "and LOCATION_ID = " << aLocationKey;

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn( "SCEREP_ID" );

		std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
        bool reportAlreadyExists = false;
		
        if (data->getNumRows() == 0) // No equipment reports found
			reportAlreadyExists = false;
		else
			reportAlreadyExists = true;

	    delete data;
        data = NULL;

		return reportAlreadyExists;		
	}
} // TA_Base_Core
