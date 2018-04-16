/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/occi_database/src/OcciDatabase.cpp $
 * @author:  Robert Stagg
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include <occi.h>

#include "OcciDatabase.h"
#include "OcciSqlExceptionHandler.h"


OcciDatabase::OcciDatabase()
    : m_pool(0),
      m_env(0)
{
    FUNCTION_ENTRY("OcciDatabase");

    try
    {
        m_env = Environment::createEnvironment(Environment::THREADED_MUTEXED);
    }
    catch (SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
}

OcciDatabase::~OcciDatabase()
{
    FUNCTION_ENTRY("~OcciDatabase");

    try
    {
        Environment::terminateEnvironment(m_env);
        m_env = 0;
    }
    catch (SQLException& ex)
    {
        m_env = 0;
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
}

shared_ptr<OcciConnection> OcciDatabase::connect(const std::string& user, const std::string& pass, const std::string& database)
{
    FUNCTION_ENTRY("connect");

	//Creates a connection pool if one does
	//not exist.
	if ( 0 == m_pool )
	{
	    m_pool = new OcciConnectionPool(database, user, pass);
	}

	//Returns a OcciConnection from the pool
    FUNCTION_EXIT;
    return m_pool->open(m_env);

}

void OcciDatabase::disconnect()
{
    FUNCTION_ENTRY("disconnect");

    FUNCTION_EXIT;
}