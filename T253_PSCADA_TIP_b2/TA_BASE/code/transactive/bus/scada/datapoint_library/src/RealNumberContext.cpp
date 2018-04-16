/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/datapoint_library/src/RealNumberContext.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2017/10/31 16:21:54 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *	Real Number Context...Required by the Mathematical_Evaluation library
  *
  */


#include "core/utilities/src/DebugUtil.h"
#include "bus/scada/datapoint_library/src/RealNumberContext.h"


namespace TA_Base_Bus
{

	RealNumberContext::RealNumberContext()
	:
	CommonContext()
	{

	}


	RealNumberContext::RealNumberContext ( IExpressionNodeNotification * userOfExpression )
	:
	CommonContext ( userOfExpression )
	{

	}


	RealNumberContext::~RealNumberContext()
	{

	}


	double RealNumberContext::getValue ( const std::string & variableName ) const
	{
        double result = const_cast< RealNumberContext * >(this)->getDoubleValue ( variableName );

        //TD16581
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getValue(%s) = %f", variableName.c_str(), result);

        return result;
	}  

}
