/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/RegSubQueryCommand.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2017/10/31 16:21:54 $
 * Last modified by:  $Author: CM $
 *
 * Region or Subsystem commands that take a query parameter. There is only one
 * so a member function pointer is not required as a constructor
 * parameter.
 *
 */

#ifndef REGSUBQUERYCOMMAND_H_INCLUDED
#define REGSUBQUERYCOMMAND_H_INCLUDED

namespace TA_Base_Bus
{
	template < class RegSub >
    class RegSubQueryCommand : public IRegSubCommand<RegSub>
    {
    public:
		RegSubQueryCommand( DutyState& state, IDutyQuery& query );

		virtual void execute(RegSub& regSub, bool doApply);

	private:
		DutyState&  m_state;
		IDutyQuery& m_query;
    };

	template < class RegSub >
	RegSubQueryCommand<RegSub>::RegSubQueryCommand( DutyState& state, IDutyQuery& query ) :
		m_state(state),
		m_query(query)
	{
	}

	template < class RegSub >
	void RegSubQueryCommand<RegSub>::execute(RegSub& regSub, bool doApply)
	{
		regSub.executeQuery( m_state, m_query );

// 		if( doApply )
// 		{
// 			regSub.apply(m_state);
// 		}
	}
}

#endif // REGSUBQUERYCOMMAND_H_INCLUDED
