/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File$
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * This class implements the TA_Base_App::MonitorExecutive interface.
 */

#include "DutyAgent.h"
#include "MonitorExecutive.h"

namespace TA_Base_App
{
	void MonitorExecutive::execute( IDutyAgentCommand& command, TA_Base_Bus::RightsLibrary& rightsLib, ta_uint32 resourceKey, bool checkRights/*=true*/ ) const
	{
		throw( TA_Base_Core::OperationModeException(RPARAM_MONITOR) );
	}

}
