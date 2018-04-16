/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/FZL1_TIP/FZL1/transactive/app/bas/BASAgent/src/ITimetableExecutionProxy.h $
  * @author:	Huang Jian
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/19 17:59:10 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */
#if !defined(GZL6_3002_app_bas_BASAgent_ITimetableExecutionProxy_H)
#define GZL6_3002_app_bas_BASAgent_ITimetableExecutionProxy_H

#include "core/data_access_interface/entity_access/src/StationBASAgentEntityData.h"
#include "bus/bas/bas_agent/IDL/src/IStationBasAgentCorbaDef.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "core/data_access_interface/bas/src/BASTimeTableMappingAccessFactory.h"

namespace TA_IRS_App
{
    class ITimetableExecutionProxy
    {
    public:
        virtual void executeTimeTable(TA_IRS_Bus::IStationBasAgentCorbaDef::ETimeTableType timeTableType, CORBA::Long tableKey, const char* sessionID, long locationKey) = 0;
        virtual void executeScheduleTimeTable(const std::string& weekday) = 0;
        virtual void setControlMode(bool bControlMode) = 0;
        virtual bool isAgentRunningControlMode() = 0;
        virtual ~ITimetableExecutionProxy(){};
    };
}


#endif