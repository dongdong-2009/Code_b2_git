// InfoTabHelper.cpp: implementation of the InfoTabHelper class.

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/inspector_panel/src/InfoTabHelper.cpp $
  * @author:  Wen Ching
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  */

//////////////////////////////////////////////////////////////////////

#include "app/scada/Inspector_Panel/src/InfoTabHelper.h"


namespace TA_Base_App
{

	bool		InfoTabHelper::m_showQuality =  false;
	bool		InfoTabHelper::m_showTimestamp = false;
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	InfoTabHelper::InfoTabHelper()
	{
		
	}

	InfoTabHelper::~InfoTabHelper()
	{

	}

	void InfoTabHelper::setShowQuality(bool bShow)
	{
		m_showQuality = bShow;
	}

	void InfoTabHelper::setShowTimestamp(bool bShow)
	{
		m_showTimestamp = bShow;
	}

	bool InfoTabHelper::getShowQuality()
	{
		return m_showQuality;
	}

	bool InfoTabHelper::getShowTimestamp()
	{
		return m_showTimestamp;
	}
}






