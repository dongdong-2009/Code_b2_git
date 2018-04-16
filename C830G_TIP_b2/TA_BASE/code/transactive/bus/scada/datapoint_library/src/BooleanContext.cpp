/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datapoint_library/src/BooleanContext.cpp $
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

