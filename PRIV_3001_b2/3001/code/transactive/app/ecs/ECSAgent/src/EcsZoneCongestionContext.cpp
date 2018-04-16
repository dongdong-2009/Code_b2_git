/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/EcsZoneCongestionContext.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
///////////////////////////////////////////////////////////
//
//  EcsZoneCongestionContext.cpp
//  Implementation of the Class EcsZoneCongestionContext
//  Generated by Enterprise Architect
//  Created on:      25-Sep-2003 18:53:34
//  Original author:
//
///////////////////////////////////////////////////////////
//  Modification history:
//
//
///////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER


#include "app/ecs/ECSAgent/src/EcsZoneCongestionContext.h"
#include "app/ecs/ECSAgent/src/ItaEcsDatapoints.h"
#include "app/ecs/ECSAgent/src/EcsGlobals.h"

#include "core/utilities/src/DebugUtil.h"


// ToDo - congestion expressions need to be prevalidated for the format "ZONE<X>"

EcsZoneCongestionContext::EcsZoneCongestionContext(ItaEcsDatapoints& theDatapoints)
: m_theDatapoints(theDatapoints)
{
    FUNCTION_ENTRY("Constructor");
    FUNCTION_EXIT;
}


// ExceptionChecked
EcsZoneCongestionContext::~EcsZoneCongestionContext()
{
    FUNCTION_ENTRY("Destructor");
    FUNCTION_EXIT;
}

bool EcsZoneCongestionContext::getValue(const std::string& variableName) const
{
    FUNCTION_ENTRY("getValue");
	// Expecting expression variable of the form "Zn" where n is the ECS Zone Id

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"Obtaining EcsZoneCongestionContext %s value", variableName.c_str());

   
    std::string::size_type startOfZoneId = variableName.find_first_of(NUMERALS);

    if (startOfZoneId == std::string::npos)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Cannot interpret Boolean Context - assuming False", variableName.c_str());
	    FUNCTION_EXIT;
		return false;
    }

    std::string::size_type endOfZoneId = variableName.find_first_not_of(NUMERALS, startOfZoneId);

    std::string zoneId;

    if (endOfZoneId == std::string::npos)
    {
        zoneId = variableName.substr(startOfZoneId);
    }
    else
    {
        zoneId = variableName.substr(startOfZoneId, endOfZoneId-startOfZoneId);
    }

    const unsigned long ulCongestion = m_theDatapoints.getCongestion(atol(zoneId.c_str()));

	return ((ulCongestion == TA_IRS_Bus::ECS_ZONE_CONGESTION_ENUM_STALLED_CONGESTION) || 
        (ulCongestion == TA_IRS_Bus::ECS_ZONE_CONGESTION_ENUM_HITEMP_CONGESTION));
}
