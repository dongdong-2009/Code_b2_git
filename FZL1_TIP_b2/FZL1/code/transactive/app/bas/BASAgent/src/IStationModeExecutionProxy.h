/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/FZL1_TIP/FZL1/transactive/app/bas/BASAgent/src/IStationModeExecutionProxy.h $
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
#if !defined(GZL6_3002_app_bas_BASAgent_IStationModeExecutionProxy_H)
#define GZL6_3002_app_bas_BASAgent_IStationModeExecutionProxy_H

#include "bus/bas/bas_agent/IDL/src/IStationBasAgentCorbaDef.h"

namespace TA_IRS_App
{
    class IStationModeExecutionProxy
    {
    public:
        virtual void executeStationMode(CORBA::Long subsystemKey, CORBA::Long stationMode, CORBA::Boolean canOverrideStationModeInProgress, const char* sessionId) = 0;
        virtual bool isAgentRunningControlMode() = 0;
        virtual void setControlMode(bool bControlMode) = 0;
        virtual ~IStationModeExecutionProxy(){}
    };
}

#endif