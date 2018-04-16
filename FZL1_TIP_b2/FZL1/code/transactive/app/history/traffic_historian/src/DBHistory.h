/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/traffic_historian/src/DBHistory.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// CDBHistory.h: implementation of the CGetPlotParams class.
// Author: Craig Grant
//
// Used to get the insert values into the history table.  A singleton
// that opens a connection to the database and keeps it open ready to
// write history records as required.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CDBHISTORY_H__7BC911BD_CA43_427F_9C19_52FB3D71CA65__INCLUDED_)
#define AFX_CDBHISTORY_H__7BC911BD_CA43_427F_9C19_52FB3D71CA65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SimpleDb.h>
#include <string>

// Traffic detector history parameter class; A singleton class
// Contains an array of the key values from the EntityHistoryParams table
// for the traffic detector parameters.  This is used to map the parameters
// to the database key values.
class CDBHistory
{
public:
	static CDBHistory* Instance();
	~CDBHistory();

	bool WriteHistoryRecord( const char* sysDate, long entityId,
							 long valueKey, long lane,	long value );

protected:
	// Constuctor
	CDBHistory();

private:
	SimpleDb *db;
};

#endif // !defined(AFX_CDBHISTORY_H__7BC911BD_CA43_427F_9C19_52FB3D71CA65__INCLUDED_)
