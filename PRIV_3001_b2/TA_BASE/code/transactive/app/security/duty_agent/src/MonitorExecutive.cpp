/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/app/security/duty_agent/src/MonitorExecutive.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 *
 * This class implements the TA_Base_App::MonitorExecutive interface.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/MonitorExecutive.h"

namespace TA_Base_App
{
	void MonitorExecutive::execute( IDutyAgentCommand& command, TA_Base_Bus::RightsLibrary& rightsLib, ta_uint32 resourceKey, bool checkRights/*=true*/ ) const
	{
		throw( TA_Base_Core::OperationModeException(RPARAM_MONITOR) );
	}

}
