/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/traffic_historian/src/DBConnection.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// DBConnection.h: Implementation of the DBConnection class
// A singleton class that retrieves the database connection string
// from the process manager.
//
// Author: Craig Grant 20/02/2001

//
// Ignore STL warnings on Windows.
//
#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include <CorbaUtil.h>
#include <DebugUtil.h>
#include <SimpleDb.h>

#include "DBConnection.h"

// Implementation of singleton
DBConnection* DBConnection::Instance()
{
	static DBConnection dbConnection;
	return &dbConnection;
}

DBConnection::DBConnection()
{
	// Retrieve the database connection string from the process manager
	std::string param = RunParams::GetInstance().Get(RPARAM_DBCONNECTION);
	if (param.empty())
	{
        LOG(DebugUtil::DebugFatal, "No database connection parameter");
	}

	// Extract the database name
	int pos1 = param.find( ':' );
	assert( pos1 != (int)std::string::npos );
	m_DBName = param.substr(0, pos1);

	// Extract the username
	int pos2 = param.find( ':', pos1+1 );
	assert( pos2 != (int)std::string::npos );
	assert( pos2 > pos1 );
	m_Username = param.substr( pos1+1, pos2 - pos1 - 1 );

	// Extract the password
	int pos3 = param.find( ':', pos2+1 );
	m_Password = param.substr( pos2+1, pos3 - pos2 - 1 );
}

DBConnection::~DBConnection()
{
}

// Test the database connection, return TRUE if OK.
bool DBConnection::TestConnection()
{
	// Create database connection
	SimpleDb db( m_DBName.c_str(), m_Username.c_str(), m_Password.c_str() );

	if( !db.Open() )
	{
        LOG(DebugUtil::DebugFatal, "Error: Cannot open database connection");
		return false;
	}
	return true;
}
