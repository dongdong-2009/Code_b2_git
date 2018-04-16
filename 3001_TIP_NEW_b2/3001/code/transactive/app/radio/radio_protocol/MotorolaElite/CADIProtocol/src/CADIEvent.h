
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEvent.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */

#ifndef __CADI_Event_H
#define __CADI_Event_H


#include "ace/Method_Request.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADITypes.h"


namespace TA_IRS_App
{

    class CADIEvent : public ACE_Method_Request
    {
    public:
        CADIEvent();
        virtual ~CADIEvent();
        
        virtual int call() = 0;

    private:
        // disabled methods
        CADIEvent(const CADIEvent&);
        void operator=(const CADIEvent&);

    };


	// Afffiliation
    class AffiliationEvent : public CADIEvent  
    {
    public:
        AffiliationEvent(const CADI_RADIO_EVENT_INFO & x) : m_event(x) {};
        virtual ~AffiliationEvent();
        int call();

    private:
        CADI_RADIO_EVENT_INFO m_event;

        // disable methods
        AffiliationEvent();
        AffiliationEvent(const AffiliationEvent&);
        void operator=(const AffiliationEvent&);
    };

	
	// Command Status
    class CmdStatusEvent : public CADIEvent  
    {
    public:
        CmdStatusEvent(const CADI_CMD_STATUS_DATA& x) : m_event(x) {};
        virtual ~CmdStatusEvent();

        int call();

    private:
        CADI_CMD_STATUS_DATA m_event;

        // disable methods
        CmdStatusEvent();
        CmdStatusEvent(const CmdStatusEvent&);
        void operator=(const CmdStatusEvent&);
    };

	
	// Deaffiliation
    class DeaffiliationEvent : public CADIEvent  
    {
    public:
        DeaffiliationEvent(const CADI_RADIO_EVENT_INFO& x) : m_event(x) {};
        virtual ~DeaffiliationEvent();

        int call();

    private:
        CADI_RADIO_EVENT_INFO m_event;

        // disable methods
        DeaffiliationEvent();
        DeaffiliationEvent(const DeaffiliationEvent&);
        void operator=(const DeaffiliationEvent&);
    };


	// Emergency Alarm
    class EmergencyAlarmEvent : public CADIEvent  
    {
    public:
        EmergencyAlarmEvent(const CADI_EA_DATA& x) : m_event(x) {};
        virtual ~EmergencyAlarmEvent();

        int call();

    private:
        CADI_EA_DATA m_event;

        // disable methods
        EmergencyAlarmEvent();
        EmergencyAlarmEvent(const EmergencyAlarmEvent&);
        void operator=(const EmergencyAlarmEvent&);
    };


	// Group Call
    class GroupCallEvent : public CADIEvent  
    {
    public:
        GroupCallEvent(const CADI_GROUP_CALL_DATA& x) : m_event(x) {};
        virtual ~GroupCallEvent();

        int call();

    private:
        CADI_GROUP_CALL_DATA m_event;
        
        // disable methods
        GroupCallEvent();
        GroupCallEvent(const GroupCallEvent&);
        void operator=(const GroupCallEvent&);
    };


	// Private Call
    class PrivateCallEvent : public CADIEvent  
    {
    public:
        PrivateCallEvent(const CADI_PRIVATE_CALL_DATA& x) : m_event(x) {};
        virtual ~PrivateCallEvent();

        int call();

    private:
        CADI_PRIVATE_CALL_DATA m_event;
        
        // disabled methods
        PrivateCallEvent();
        PrivateCallEvent(const PrivateCallEvent&);
        void operator=(const PrivateCallEvent&);
    };
	

	// Radio Ack
    class RadioAckEvent : public CADIEvent  
    {
    public:
        RadioAckEvent(const CADI_RADIO_ACK_DATA& x) : m_event(x) {};
        virtual ~RadioAckEvent();

        int call();

    private:
        CADI_RADIO_ACK_DATA m_event;

        // disabled methods
        RadioAckEvent();
        RadioAckEvent(const RadioAckEvent&);
        void operator=(const RadioAckEvent&);
    };


	// Session
    class SessionEvent : public CADIEvent  
    {
    public:
        SessionEvent(const CADI_SESSION_STATUS& x) : m_event(x) {};
        virtual ~SessionEvent();

        int call();

    private:
        CADI_SESSION_STATUS m_event;
        
        // disabled methods
        SessionEvent();
        SessionEvent(const SessionEvent&);
        void operator=(const SessionEvent&);
    };


	// Status
    class StatusEvent : public CADIEvent  
    {
    public:
        StatusEvent(const CADI_STATUS_DATA& x) : m_event(x) {};
        virtual ~StatusEvent();

        int call();

    private:
        CADI_STATUS_DATA m_event;

        // disabled methods
        StatusEvent();
        StatusEvent(const StatusEvent&);
        void operator=(const StatusEvent&);
    };

	
	// Zone Controller Status
    class ZCStatusEvent : public CADIEvent  
    {
    public:
        ZCStatusEvent(const CADI_ZC_STATUS_DATA& x) : m_event(x) {};
        virtual ~ZCStatusEvent();

        int call();

    private:
        CADI_ZC_STATUS_DATA m_event;

        // disabled methods
        ZCStatusEvent();
        ZCStatusEvent(const ZCStatusEvent&);
        void operator=(const ZCStatusEvent&);
    };
};

#endif
