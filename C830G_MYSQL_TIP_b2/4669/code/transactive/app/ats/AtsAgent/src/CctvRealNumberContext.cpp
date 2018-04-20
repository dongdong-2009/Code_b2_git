/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/CctvRealNumberContext.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2008/11/28 16:26:01 $
  * Last modified by:	$Author: builder $
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

