/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/crvtrackcursorinfo.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_CRVTRACKCURSORINFO_H__2BE774C1_E7C8_4467_B672_9C1E6422ABCD__INCLUDED_)
#define AFX_CRVTRACKCURSORINFO_H__2BE774C1_E7C8_4467_B672_9C1E6422ABCD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.

/////////////////////////////////////////////////////////////////////////////
// CCRVTrackCursorInfo wrapper class

class CCRVTrackCursorInfo : public COleDispatchDriver
{
public:
	CCRVTrackCursorInfo() {}		// Calls COleDispatchDriver default constructor
	CCRVTrackCursorInfo(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CCRVTrackCursorInfo(const CCRVTrackCursorInfo& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetDetailAreaCursor();
	void SetDetailAreaCursor(long nNewValue);
	long GetDetailAreaFieldCursor();
	void SetDetailAreaFieldCursor(long nNewValue);
	long GetGraphCursor();
	void SetGraphCursor(long nNewValue);
	long GetGroupAreaCursor();
	void SetGroupAreaCursor(long nNewValue);
	long GetGroupAreaFieldCursor();
	void SetGroupAreaFieldCursor(long nNewValue);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRVTRACKCURSORINFO_H__2BE774C1_E7C8_4467_B672_9C1E6422ABCD__INCLUDED_)
