/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/CctvRealNumberContext.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2012/02/06 16:15:14 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  *	Boolean Context...Required by the Mathematical_Evaluation library
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "app/ats/AtsAgent/src/AtsCctvData.h"
#include "app/ats/AtsAgent/src/CctvRealNumberContext.h"

namespace TA_IRS_App
{

	CctvRealNumberContext::CctvRealNumberContext ( AtsCctvData& cctvData )
	:
	m_cctvData ( cctvData )
	{

	}


	CctvRealNumberContext::~CctvRealNumberContext()
	{

	}


	double CctvRealNumberContext::getValue ( const std::string & variableName ) const
	{
		return m_cctvData.getRealNumberValue ( variableName );
	}

}

