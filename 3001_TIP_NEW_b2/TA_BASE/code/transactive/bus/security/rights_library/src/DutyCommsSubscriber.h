#ifndef DUTYCOMMSSUBSCRIBER_H_INCLUDED
#define DUTYCOMMSSUBSCRIBER_H_INCLUDED

/**
  * DutyCommsSubscriber.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/rights_library/src/DutyCommsSubscriber.h $
  * @author:  G. Kirkpatrick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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





