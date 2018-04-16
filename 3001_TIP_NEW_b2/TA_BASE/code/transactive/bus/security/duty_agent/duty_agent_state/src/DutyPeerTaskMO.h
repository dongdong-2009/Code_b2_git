/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/DutyPeerTaskMO.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/06/06 17:21:08 $
  *  Last modified by:  $Author: jinmin.huang $
  *
  *  A base class for DutyPeerTask method objects that encapsulates the error
  *  handling.
  */

#ifndef DUTYPEERTASKMO_H_INCLUDED
#define DUTYPEERTASKMO_H_INCLUDED

namespace TA_Base_Bus
{
	class DutyPeerTask;
}

namespace TA_Base_Bus
{
	//Class declaration
	class DutyPeerTaskMO : public ACE_Method_Request
	{
	// Public methods
	public:
		DutyPeerTaskMO( const ACE_Future<bool>& future );
		virtual ~DutyPeerTaskMO(){};
		
		virtual int call();

	protected:

		virtual void execute() = 0;

	private:

		DutyPeerTaskMO& operator=( const DutyPeerTaskMO& );
		DutyPeerTaskMO( const DutyPeerTaskMO& );

		ACE_Future<bool> m_future;

		static unsigned long getTimeOutSeconds();
		static const unsigned long MAX_PEER_TIMEOUT;
		static const unsigned long TIME_OF_FLIGHT;
		static unsigned long m_timeout;  //maximum time value second;
		unsigned long  m_createtime;     //created time of the task;
	}; 
} // TA_Base_Bus

#endif //DUTYPEERTASKMO_H_INCLUDED
