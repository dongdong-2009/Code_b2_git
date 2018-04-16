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
#include "app/response_plans/plan_agent/src/StringContextImpl.h"


namespace TA_Base_App
{

	StringContextImpl::StringContextImpl()
	{
	}

    StringContextImpl::StringContextImpl( const std::map<std::string, std::string>& mapExchangingData ) : 
    m_mapExchangingData( m_mapExchangingData )
	{
	}

	//StringContextImpl::StringContextImpl ( IExpressionNodeNotification * userOfExpression )
	//{

	//}

	StringContextImpl::~StringContextImpl()
	{

	}

	std::string StringContextImpl::getValue(const std::string& name) const
	{
		std::string result = "";
		std::map<std::string, std::string>::const_iterator itExchangingDataFound = m_mapExchangingData.find(name);
		if (itExchangingDataFound == m_mapExchangingData.end())
		{
			TA_THROW( TA_Base_Core::TransactiveException( "Cannot find exchangingData" ) );
		}
        result = itExchangingDataFound->second;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getValue(%s) = %s", name.c_str(), result.c_str() );

		return result;
	}

}

