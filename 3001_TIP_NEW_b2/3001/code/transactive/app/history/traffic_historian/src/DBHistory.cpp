/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/traffic_historian/src/DBHistory.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// CDBHistory.cpp: implementation of the CDBHistory class.
// Author: Craig Grant
//
// Used to get the insert values into the history table.  A singleton
// that opens a connection to the database and keeps it open ready to
// write history records as required.
//////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <DebugUtil.h>

//using namespace std;

#include "DBHistory.h"
#include "DBConnection.h"

// Implementation of singleton
CDBHistory* CDBHistory::Instance()
{
	static CDBHistory dbHistory;
	return &dbHistory;
}

// Constructor
CDBHistory::CDBHistory()
{
	// Get the database connection instance
	DBConnection* dbc = DBConnection::Instance();

	// Set up the connection
	db = new SimpleDb(dbc->GetDBName(), dbc->GetUsername(), dbc->GetPassword());

    if (!db->Open())
    {
		LOG(DebugUtil::DebugWarn, "Database open failed!" );
        return;
    }
}

CDBHistory::~CDBHistory()
{
	// Clean Up
	delete db;
}

bool CDBHistory::WriteHistoryRecord( const char* sysDate, long entityId,
									 long valueKey, long lane, long value )
{
	char buffer[201];
    std::string statement = "INSERT INTO HISTORY "
							"(H_LOG_TIME, ENTITYKEY, EHP_KEY, H_LANE, H_VALUE) "
							"VALUES ( to_date('";
	statement += sysDate;
	statement += "', 'DD/MM/YYYY HH24:MI:SS' ), ";
	sprintf( buffer, "%ld, %ld, %ld, %ld )",
			 entityId, valueKey, lane, value );
	statement += buffer;

    LOG(DebugUtil::DebugDebug, statement.c_str());
    if (!db->Exec(statement.c_str()))
    {
		LOG(DebugUtil::DebugWarn, "WriteHistoryRecord insert failed" );
        return false;
    }

	return true;
}

