/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/traffic_historian/src/HistoryParams.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// CHistoryParams.h: implementation of the CGetPlotParams class.
// Author: Craig Grant
//
// Used to get the list of parameters that can be plotted for the
// given device.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHISTORYPARAMS_H__7BC911BD_CA43_427F_9C19_52FB3D71CA65__INCLUDED_)
#define AFX_CHISTORYPARAMS_H__7BC911BD_CA43_427F_9C19_52FB3D71CA65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SimpleDb.h"
#include <string>

// Traffic detector history parameter class; A singleton class
// Contains an array of the key values from the EntityHistoryParams table
// for the traffic detector parameters.  This is used to map the parameters
// to the database key values.
class CTDHistoryParams
{
public:
	static CTDHistoryParams* Instance();

	enum VarPos
	{
        VOLUMETYPE1 = 0,
        VOLUMETYPE2,
		VOLUMETYPE3,
        OCCUPANCY,
        HEADWAY,
        SPEED
	};

	const int& GetKeyValue( const VarPos arrayPos ) const { return m_DBKey[arrayPos]; }

protected:
	// Constuctor
	CTDHistoryParams();

private:
	static CTDHistoryParams* _instance;

	int m_DBKey[6];
};


#define CF_DATA_MAX_FIELDSIZE	100
	
// Structure to contain the device information
struct CGetHistoryParamsRow  
{
	int	key;
    std::string	name;
};

// Class to retrieve the device information into CGetHistoryParamsRow structure
class CGetHistoryParams  
{
public:
	CGetHistoryParams(	const char *hostName, const char *databaseName, 
					const char *userName, const char *password);

	virtual ~CGetHistoryParams();

	// Get all rows for the given Category
	virtual bool AllRowsForCategory(std::string categoryName,
									CGetHistoryParamsRow &row);

	// Get next row (must do AllRows or GetRow first)
	virtual bool NextRow( CGetHistoryParamsRow &row);

    virtual bool open(){return m_dbConnection.Open();}

    virtual void close(){m_dbConnection.Close();}

    virtual bool isOpen(){return m_dbConnection.IsOpen();}

    std::string TableName(){return m_tableName;}

private:
    SimpleDb m_dbConnection;
    SimpleDb::Buffer m_buffer;
    std::string m_tableName;
};

#endif // !defined(AFX_CHISTORYPARAMS_H__7BC911BD_CA43_427F_9C19_52FB3D71CA65__INCLUDED_)
