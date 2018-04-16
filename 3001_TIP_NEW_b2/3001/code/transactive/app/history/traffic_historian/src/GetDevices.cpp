/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/traffic_historian/src/GetDevices.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// DBDeviceView.cpp: implementation of the DBDeviceView class.
// Author: Craig Grant
//
// Used to get the rows from HISTORY_DEVICE_V which is a view of the
// devices used by the history program.
//
// Modified by: 
//
// Katherine Thomson 21/02/02 : Changed NUM_LOOPS to NUM_LANES
//                              Removed HISTORY_LOG_INT
//
//////////////////////////////////////////////////////////////////////
#include "GetDevices.h"
//#include "DBError.h"
#include <DebugUtil.h>

// Constructor
DBDeviceView::DBDeviceView(
    const char *hostName,
    const char *databaseName,
    const char *userName,
    const char *password)
    : m_dbConnection( databaseName, userName, password, hostName ),
      m_tableName("HISTORY_DEVICES_V")
{

}

// Destructor
DBDeviceView::~DBDeviceView()
{
}

// Get all the rows from the view
bool DBDeviceView::AllRows(DBDeviceViewRow &row)
{
    std::string statement = "SELECT DEVICE_NAME, DEVICE_KEY, NUM_LANES FROM ";
    statement +=  m_tableName;

    m_buffer.clear();
    if(!m_dbConnection.Exec(statement.c_str(), 3, m_buffer))
    {
        LOG(DebugUtil::DebugError, "Database Error. Failing query:");
        LOGMORE(DebugUtil::DebugError, statement.c_str());
        return false;
    }

    if(m_buffer.size() == 0)
    {
        LOG(DebugUtil::DebugError, "Query return no data.");
        LOGMORE(DebugUtil::DebugError, statement.c_str());
        return false;
    }

    row.name = m_buffer[0][0];
    row.key = strtoul(m_buffer[0][1].c_str(), NULL, 10);
    row.lanes = m_buffer[0][2];
    m_buffer.erase(m_buffer.begin());

    return true;
}

// Get row(s) with the given value
bool DBDeviceView::GetRow(std::string keyValue, std::string keyName, DBDeviceViewRow &row)
{

    std::string statement    = "SELECT DEVICE_NAME, DEVICE_KEY, NUM_LANES FROM ";
    statement            +=    m_tableName;
    statement            +=    " WHERE ";
    statement            +=    keyName;
    statement            +=    " = '";
    statement            +=    keyValue;
    statement            +=    "'";

    // To write a row use bindType BY_DEFINE_POS
    if(!m_dbConnection.Exec(statement.c_str(), 3, m_buffer))
    {
        LOG(DebugUtil::DebugError, "Database Error. Failing query:");
        LOGMORE(DebugUtil::DebugError, statement.c_str());
        return false;
    }

    if(m_buffer.size() == 0)
    {
        LOG(DebugUtil::DebugError, "Query return no data.");
        LOGMORE(DebugUtil::DebugError, statement.c_str());
        return false;
    }

    row.name = m_buffer[0][0];
    row.key = strtoul(m_buffer[0][1].c_str(), NULL, 10);
    row.lanes = m_buffer[0][2];
    m_buffer.erase(m_buffer.begin());

    return true;

}

// Get the next row of information
bool DBDeviceView::NextRow(DBDeviceViewRow &row)
{
   // is there more data in the buffer?
    if(m_buffer.size() == 0)
    {
        // is the more to fetch?
        if(m_dbConnection.HasMore())
        {
            m_buffer.clear();
            m_dbConnection.FetchMore(3, m_buffer);
        }
        // no more data
        else
        {
            return false;
        }
    }

    // get the next row
    row.name = m_buffer[0][0];
    row.key = strtoul(m_buffer[0][1].c_str(), NULL, 10);
    row.lanes = m_buffer[0][2];
    m_buffer.erase(m_buffer.begin());

    return true;
}


