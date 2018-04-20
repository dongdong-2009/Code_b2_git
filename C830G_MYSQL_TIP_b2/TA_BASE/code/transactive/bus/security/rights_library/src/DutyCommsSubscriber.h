#ifndef DUTYCOMMSSUBSCRIBER_H_INCLUDED
#define DUTYCOMMSSUBSCRIBER_H_INCLUDED

/**
  * DutyCommsSubscriber.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  G. Kirkpatrick
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This class listens for the Duty Agent comms message containing updates for subsystem duty.
  *
  */

namespace TA_Base_Bus
{
    class DutyCommsSubscriber : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {
    public:

        DutyCommsSubscriber( RightsCache& rightsCache );

		~DutyCommsSubscriber();

        virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

    private:

        DutyCommsSubscriber(const DutyCommsSubscriber&);
	    DutyCommsSubscriber& operator=(const DutyCommsSubscriber &);

		bool m_firstUpdate; 

		CORBA::ULong m_version;

		RightsCache& m_rightsCache;
    };

} // TA_Base_App

#endif // DUTYCOMMSSUBSCRIBER_H_INCLUDED





