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
#include "app/response_plans/plan_agent/src/RealNumberContextImpl.h"


namespace TA_Base_App
{

	RealNumberContextImpl::RealNumberContextImpl()
	{
	}

    RealNumberContextImpl::RealNumberContextImpl( const std::map<std::string, std::string>& mapExchangingData ) :
    m_mapExchangingData( mapExchangingData )
	{
       m_mapExchangingData = mapExchangingData;
	}

	//StringContextImpl::StringContextImpl ( IExpressionNodeNotification * userOfExpression )
	//{

	//}

	RealNumberContextImpl::~RealNumberContextImpl()
	{

	}

	double RealNumberContextImpl::getValue(const std::string& variableName) const
	{
		double result = 0;
		std::map<std::string, std::string>::const_iterator itExchangingDataFound = m_mapExchangingData.find(variableName);
		if (itExchangingDataFound == m_mapExchangingData.end())
		{
			TA_THROW( TA_Base_Core::TransactiveException( "Cannot find exchangingData" ) );
		}
        result = (double)atoi(itExchangingDataFound->second.c_str());

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getValue(%s) = %d", variableName.c_str(), result );

		return result;
	}

}

