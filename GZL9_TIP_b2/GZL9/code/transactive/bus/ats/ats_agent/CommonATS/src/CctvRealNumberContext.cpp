/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/4669_Review_Branch/4669/transactive/bus/ats/ats_agent/CommonATS/src/CctvRealNumberContext.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #3 $
  *
  * Last modification:	$DateTime: 2010/03/30 11:17:43 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	Boolean Context...Required by the Mathematical_Evaluation library
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsCctvData.h"
#include "bus/ats/ats_agent/CommonATS/src/CctvRealNumberContext.h"

namespace TA_IRS_Bus
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

