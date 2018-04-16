/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/IStationModeExecutionProxy.h $
  * @author:	Huang Jian
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/02/27 17:12:34 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */
#if !defined(APP_BAS_BASAgent_IStationModeExecutionProxy_H)
#define APP_BAS_BASAgent_IStationModeExecutionProxy_H

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