#ifndef RIGHTSQUERY_H_INCLUDED
#define RIGHTSQUERY_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Caches rights.
  */
namespace TA_Base_Bus
{
	/**
	  * This class creates a connection to the Rights Agent 
	  * every time it is instantiated. To improve performace
	  * consider creating and maintaining one per thread.
	  *
	  */
	class RightsQuery
	{
	public:
		RightsQuery( const std::string& session, unsigned long location,
			unsigned long subsystem, unsigned long action, bool result,
			const std::string& reason, EDecisionModule module );

		RightsQuery( const std::string& session, unsigned long location,
			unsigned long subsystem, unsigned long action );

		RightsQuery( const std::string& session, unsigned long location,
			unsigned long subsystem );

		RightsQuery( const std::string& session );

		RightsQuery(unsigned long location);

		bool operator==( const RightsQuery& rhs ) const;
		bool operator< ( const RightsQuery& rhs ) const;

		std::string m_session;
		unsigned long m_location;
		unsigned long m_subsystem;
		unsigned long m_action;
		mutable bool m_result;
		std::string m_reason;
		EDecisionModule m_module;
	};
}

#endif // RIGHTSQUERY_H_INCLUDED
