/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/traffic_historian/src/GetDevices.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// DBDeviceView.h: implementation of the DBDeviceView class.
// Author: Craig Grant
//
// Used to get the rows from HISTORY_DEVICE_V which is a view of the
// devices used by the history program.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBDEVICEVIEW_H__7BC911BD_CA43_427F_9C19_52FB3D71CA65__INCLUDED_)
#define AFX_DBDEVICEVIEW_H__7BC911BD_CA43_427F_9C19_52FB3D71CA65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SimpleDb.h"
#include <string>
#include <vector>

#define CF_DATA_MAX_FIELDSIZE	100
	
// Structure to contain the device information
struct DBDeviceViewRow_s  
{
    std::string	 name;	     // Entity name
	int		     key;		 // Entity key
    std::string	 lanes;	     // Number of lanes
};

typedef struct DBDeviceViewRow_s	DBDeviceViewRow;

// Class to retrieve the device information into DBDeviceViewRow_s structure
// based on the OCIAccess class in DataAccessLib
class DBDeviceView
{
public:
	DBDeviceView(	const char *hostName, const char *databaseName, 
					const char *userName, const char *password);

	virtual ~DBDeviceView();

	// Get all the rows from the view
	virtual bool AllRows(	DBDeviceViewRow &row);

	// Get row(s) for a particular value
	virtual bool GetRow(    std::string keyValue, std::string keyName, 
                            DBDeviceViewRow &row);

	// Get next row (must do AllRows or GetRow first)
	virtual bool NextRow(   DBDeviceViewRow &row);

    bool open(){return m_dbConnection.Open();}

    bool isOpen(){return m_dbConnection.IsOpen();}

    void close(){m_dbConnection.Close();}

private:
    SimpleDb m_dbConnection;
    std::string m_tableName;
    SimpleDb::Buffer m_buffer;
};

#endif // !defined(AFX_DBDEVICEVIEW_H__7BC911BD_CA43_427F_9C19_52FB3D71CA65__INCLUDED_)
