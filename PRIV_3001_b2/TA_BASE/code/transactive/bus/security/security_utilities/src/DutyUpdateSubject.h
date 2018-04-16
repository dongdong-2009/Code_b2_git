#ifndef DUTYUPDATESUBJECT_H_INCLUDED
#define DUTYUPDATESUBJECT_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/security_utilities/src/DutyUpdateSubject.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Receives duty updates from the Duty Agent and publishes
  * the update using the observer pattern.
  *
  */

namespace TA_Base_Bus
{
	class DutyUpdateSubject : public SeSubject<DutyUpdateSubject>, 
		                         public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
	{
	public:
		DutyUpdateSubject();
		~DutyUpdateSubject();

		void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);
		void interpretUpdate( IDutyUpdateObserver& observer );

	private:
		//DutyUpdateSubject(const DutyUpdateSubject& dutyUpdateSubject);

		void changed   ( IDutyUpdateObserver& observer );

		TA_Base_Bus::DutyAgentTypeCorbaDef::SessionId m_sessionId;
		void (DutyUpdateSubject::*m_updateFn)( IDutyUpdateObserver& observer );
	};
}

#endif // SESSIONUPDATESUBJECT_H_INCLUDED
