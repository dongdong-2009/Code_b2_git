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
 * Region or Subsystem commands that operatate on all profiles associated with a session.
 *
 */

#ifndef REGSUBMULTIPROFCOMMAND_H_INCLUDED
#define REGSUBMULTIPROFCOMMAND_H_INCLUDED

namespace TA_Base_Bus
{
	template< class RegSub >
    class RegSubMultiProfCommand : public IRegSubCommand<RegSub>
    {
    public:
		RegSubMultiProfCommand( void (RegSub::*stateCmd)(DutyState&, const DutyPrimitive&),
			DutyState& state, const TA_Base_Bus::SessionInfo& session );

		virtual void execute(RegSub& regSub, bool doApply);

	private:
		void (RegSub::*m_stateCmd)(DutyState&, const DutyPrimitive&);
		DutyState& m_state;
		const TA_Base_Bus::SessionInfo& m_session;
    };

	template < class RegSub >
	RegSubMultiProfCommand<RegSub>::RegSubMultiProfCommand( void (RegSub::*stateCmd)(DutyState&, const DutyPrimitive&),
		DutyState& state, const TA_Base_Bus::SessionInfo& session ) :
		m_stateCmd(stateCmd),
		m_state(state),
		m_session(session)
	{
	}

	template < class RegSub >
	void RegSubMultiProfCommand<RegSub>::execute(RegSub& regSub, bool doApply)
	{
		std::vector<DASType::DbKey> profiles;
		std::vector<DASType::DbKey>::const_iterator prfIt;

		try
		{
			//DutySessionCache::instance()->getProfiles( m_session, profiles );
			profiles = m_session.ProfileId;

			for( prfIt = profiles.begin(); prfIt != profiles.end(); prfIt++ )
			{
				DutyPrimitive dp(m_session.SessionId,*prfIt);

				(regSub.*m_stateCmd)( m_state, dp );
			}

// 			if( doApply )
// 			{
// 				regSub.apply(m_state);
// 			}
		}
		catch( const TA_Base_Core::DutyAgentException& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "RegSubMultiProfCommand aborted." );
		}

		
	}
}

#endif // REGSUBMULTIPROFCOMMAND_H_INCLUDED
