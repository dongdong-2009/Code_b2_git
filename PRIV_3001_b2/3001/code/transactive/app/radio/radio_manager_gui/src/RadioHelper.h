/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioHelper.h $
  * @author:  Jiang Haipeng
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * refactoring extract common functions to a class.
  *
  */

// RadioHelper.h: interface for the CRadioHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RADIOHELPER_H__2CAD9469_7BF6_471F_B928_EBBDD929CC87__INCLUDED_)
#define AFX_RADIOHELPER_H__2CAD9469_7BF6_471F_B928_EBBDD929CC87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/radio/radio_bus/src/RadioTypes.h"

class CRadioHelper  
{
public:
	static CString getTypeText(RadioResourceType type);
	static std::string timeToString(const time_t t);

private:
    // disabled methods
    CRadioHelper();
	~CRadioHelper();

};

#endif // !defined(AFX_RADIOHELPER_H__2CAD9469_7BF6_471F_B928_EBBDD929CC87__INCLUDED_)

