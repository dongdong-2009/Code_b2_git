/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/pa/PAManager/src/IAdhocMessageRecordPage.h $
  * @author Sean Liew
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  */

#if !defined(AFX_IStationBroadcastPage_H__1E0C60D7_EA0F_46fa_B38E_0190F98F0C87__INCLUDED_)
#define AFX_IStationBroadcastPage_H__1E0C60D7_EA0F_46fa_B38E_0190F98F0C87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IAdhocMessageRecordPage
{

public:
    virtual void addStringToStatusWindow( const std::string& str ) = 0;
};

#endif // !defined(AFX_IStationBroadcastPage_H__1E0C60D7_EA0F_46fa_B38E_0190F98F0C87__INCLUDED_)