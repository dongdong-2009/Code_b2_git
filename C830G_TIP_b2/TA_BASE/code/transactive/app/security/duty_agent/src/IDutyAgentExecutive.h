#ifndef IDUTYAGENTEXECUTIVE_H_INCLUDED
#define IDUTYAGENTEXECUTIVE_H_INCLUDED

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
 * Used to allow different behaviour for the execution of Agent methods between
 * Control and Monitor modes.
 */

namespace TA_Base_App
{
	class IDutyAgentCommand;

    class IDutyAgentExecutive
    {
    public:
		
		virtual ~IDutyAgentExecutive(){}

		virtual void execute(IDutyAgentCommand& command, TA_Base_Bus::RightsLibrary& rightsLib, ta_uint32 resourceKey, bool checkRights=true ) const = 0;

    };
}

#endif // IDUTYAGENTEXECUTIVE_H_INCLUDED
