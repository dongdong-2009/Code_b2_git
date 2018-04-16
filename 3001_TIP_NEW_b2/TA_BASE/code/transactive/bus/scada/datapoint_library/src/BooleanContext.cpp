/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datapoint_library/src/BooleanContext.cpp $
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
#include "bus/scada/datapoint_library/src/BooleanContext.h"


namespace TA_Base_Bus
{

	BooleanContext::BooleanContext()
	:
	CommonContext()
	{

	}


	BooleanContext::BooleanContext ( IExpressionNodeNotification * userOfExpression )
	:
	CommonContext ( userOfExpression )
	{

	}


	BooleanContext::~BooleanContext()
	{

	}


	bool BooleanContext::getValue ( const std::string & variableName ) const
	{
        bool result = const_cast< BooleanContext * >(this)->getBooleanValue ( variableName );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getValue(%s) = %s", variableName.c_str(), result ? "true" : "false" );

		return result;
	}

}

