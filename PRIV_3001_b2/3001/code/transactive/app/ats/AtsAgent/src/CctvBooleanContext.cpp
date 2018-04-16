/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/CctvBooleanContext.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2018/03/14 13:51:20 $
  * Last modified by:	$Author: lim.cy $
  *
  * Description:
  *
  *	Boolean Context...Required by the Mathematical_Evaluation library
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "app/ats/AtsAgent/src/AtsCctvData.h"
#include "app/ats/AtsAgent/src/CctvBooleanContext.h"


namespace TA_IRS_App
{

	CctvBooleanContext::CctvBooleanContext ( AtsCctvData& cctvData )
	: m_cctvData ( cctvData )
	{
	}


	CctvBooleanContext::~CctvBooleanContext()
	{
	}


	bool CctvBooleanContext::getValue ( const std::string & variableName ) const
	{
		return m_cctvData.getBooleanValue(variableName);
	}

}

