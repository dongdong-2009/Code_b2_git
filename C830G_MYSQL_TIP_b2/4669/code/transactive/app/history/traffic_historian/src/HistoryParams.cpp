/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/traffic_historian/src/HistoryParams.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// CHistoryParams.cpp: implementation of the CHistoryParams class.
// Author: Craig Grant
//
// Used to get the list of parameters that can be plotted for the
// given device.
//////////////////////////////////////////////////////////////////////

// Ignore STL warnings on Windows.

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

//using namespace std;

#include "HistoryParams.h"
//#include "DBError.h"
#include "DBConnection.h"
#include "DebugUtil.h"

// ============================== CHistoryParams ==============================

// Implementation of singleton
CTDHistoryParams* CTDHistoryParams::_instance = 0;

CTDHistoryParams* CTDHistoryParams::Instance()
{
    LOG(DebugUtil::DebugDebug, "");
	if (_instance == 0)
	{
		_instance = new CTDHistoryParams;
	}

	return _instance;
}

// Constructor
CTDHistoryParams::CTDHistoryParams()
{
    //
  	// Get the database connection instance
    //
    LOG(DebugUtil::DebugDebug, "");
	DBConnection* dbc = DBConnection::Instance();

    LOG(DebugUtil::DebugDebug, "");
	// Set up the connection
	CGetHistoryParams histParams( dbc->GetDBName(), dbc->GetDBName(), dbc->GetUsername(), 
                                  dbc->GetPassword());

    LOG(DebugUtil::DebugDebug, "");
	// Initialise the array with error values.
	for( int i=0; i<6; i++ )
		m_DBKey[i] = -1;

    LOG(DebugUtil::DebugDebug, "");
	// Open the database connection
	histParams.open();
	if ( !histParams.isOpen() )
	{
        LOG(DebugUtil::DebugDebug, "Could not open database");
		return;
	}
    LOG(DebugUtil::DebugDebug, "Database is open");
	// Do select to return all rows for the given entity type
	bool rc;
	CGetHistoryParamsRow histParamsRow;

	rc = histParams.AllRowsForCategory( "Traffic_Station", histParamsRow);

	// Go through the rows and add them to the list
	while (rc == true) // Add the last row retrieved
	{
        LOG(DebugUtil::DebugDebug, "History param name = ");
        LOGMORE(DebugUtil::DebugDebug, histParamsRow.name.c_str());
		// Save the key in the appropriate array position (ugly but quick)
		if( strcmp( histParamsRow.name.c_str(), "Volume Type 1" ) == 0 )
			m_DBKey[VOLUMETYPE1] = histParamsRow.key;
		else if( strcmp( histParamsRow.name.c_str(), "Volume Type 2" ) == 0 )
			m_DBKey[VOLUMETYPE2] = histParamsRow.key;
		else if( strcmp( histParamsRow.name.c_str(), "Volume Type 3" ) == 0 )
			m_DBKey[VOLUMETYPE3] = histParamsRow.key;
		else if( strcmp( histParamsRow.name.c_str(), "Occupancy" ) == 0 )
			m_DBKey[OCCUPANCY] = histParamsRow.key;
		else if( strcmp( histParamsRow.name.c_str(), "Headway" ) == 0 )
			m_DBKey[HEADWAY] = histParamsRow.key;
		else if( strcmp( histParamsRow.name.c_str(), "Speed" ) == 0 )
			m_DBKey[SPEED] = histParamsRow.key;
		else
            std::cout << "Unidentified parameter: " << histParamsRow.name.c_str() << std::endl;

		// Retrieve another row
		rc = histParams.NextRow(histParamsRow);
	}

	// Close the database connection
	histParams.close();
}


// ============================= CGetHistoryParams ============================

// Constructor
CGetHistoryParams::CGetHistoryParams( const char *hostName, const char *databaseName, 
									  const char *userName, const char *password )
	: m_dbConnection(databaseName, userName, password, hostName), m_tableName("ENTITYHISTORYPARAMS")
{
	
}

// Destructor
CGetHistoryParams::~CGetHistoryParams()
{
}

// Get all the rows from the table for this category
bool CGetHistoryParams::AllRowsForCategory(std::string categoryName,
										   CGetHistoryParamsRow &row)
{
	// Construct the select statement
	std::string statement = "SELECT EHP.EHP_KEY, EHP.EHP_NAME "
							"FROM ENTITYTYPE ET, ENTITYHISTORYPARAMS EHP "
							"WHERE EHP.TYPE_KEY=ET.PKEY AND "
							"ET.NAME = '";
	statement += categoryName;
	statement += "'";

    //clear buffer for new data
    m_buffer.clear();

	// To read a row.  Use bindType BY_DEFINE_POS
	if(!m_dbConnection.Exec(statement.c_str(), 2, m_buffer))
    {
        LOG(DebugUtil::DebugError, "Database Error. Executing query:");
        LOGMORE(DebugUtil::DebugError, statement.c_str());
        return false;
    }

    if(m_buffer.size() == 0)
    {
        LOG(DebugUtil::DebugDebug, "No data returned from database. query = ");
        LOGMORE(DebugUtil::DebugDebug, statement.c_str());
        return false;
    }
        
    row.key = strtoul(m_buffer[0][0].c_str(), NULL, 10);
    row.name = m_buffer[0][1];
    // remove this row from the buffer
    m_buffer.erase(m_buffer.begin());

    return true;
}


// Get the next row of information
bool CGetHistoryParams::NextRow(CGetHistoryParamsRow &row)
{
    // is there more data in the buffer?   
    if(m_buffer.size() == 0)
    {   
        // is the more to fetch?
        if(m_dbConnection.HasMore())
        {
            m_buffer.clear();
            m_dbConnection.FetchMore(2, m_buffer);
        }
        // no more data
        else
        {
            return false;
        }
    }

    // get the next row
    row.key = strtoul(m_buffer[0][0].c_str(), NULL, 10);
    row.name = m_buffer[0][1];
    // remove this row from the buffer
    m_buffer.erase(m_buffer.begin());

    return true;
}

