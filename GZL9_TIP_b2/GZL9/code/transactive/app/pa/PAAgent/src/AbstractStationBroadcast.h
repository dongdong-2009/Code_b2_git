/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */


#if !defined(ABSTRACTSTATIONBROADCAST__INCLUDED_)
#define ABSTRACTSTATIONBROADCAST__INCLUDED_

#include "bus/pa/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "app/pa/PAAgent/src/AbstractBroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"

namespace TA_IRS_App
{

struct ZoneStatus
{
    ZoneStatus()
        : m_sourceId(0), m_status(false), m_priority(0) 
    {}
    
    CORBA::Octet m_sourceId;
    bool m_status;
    CORBA::Octet m_priority;
};


struct MessageSequenceStatus
{    
    enum EState
    {
        NoFault                                         = 0,
        CyclicPlayingNow                                = 1,
        NotUsed2                                        = 2,
        CyclicInDwellPeriod                             = 3,
        NoDvaChannelAvailableAtStart                    = 4,
        DvaMessageCurrentlyRecording                    = 5,
        DvaChannelRequestedForHigherPriorityBroadcast   = 6,
        NotUsed3                                        = 7,
        BroadcastFinishedAbnormally                     = 8,
        BroadcastFinishedNormally                       = 9
    } m_status;
    CORBA::Octet m_announceId;

    CORBA::Octet m_sourceid;
    
    MessageSequenceStatus()
        : m_status(NoFault), 
        m_announceId(0),
        //m_priority(0)
        m_sourceid(0)
    {}
};

class AbstractStationBroadcast : public AbstractBroadcast
{

public:

    AbstractStationBroadcast(TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                             const std::string& sessionId);
        // throw CorruptInternalStateDataException


    void retry( const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const std::string& sessionId );
        // throw(TA_Base_Core::BadParameterException) ;




    virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* getBroadcastParametersCopy();
    virtual void setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter );

    virtual ~AbstractStationBroadcast();

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage getEffectiveZoneCoverage( 
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones );
    
    virtual TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage getZoneCoverage() = 0;

    TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices calculateEffectiveCoverageIndices( 
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& allZones, 
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& newZones );
        //throw(TA_Base_Core::BadParameterException)


    // Agent-synchronised BroadcastParameter
    CORBA::Octet getAnnounceId();
    void setAnnounceId(CORBA::Octet announceId, bool sendSynchronisation = true);

    // Agent-synchronised BroadcastParameter
    bool getWasExecutedSuccessfully();
    void setWasExecutedSuccessfully(bool value, bool sendSynchronisation = true);   

    virtual void updateFinalProgressReport();

	// TD17456
	virtual bool isAllFailureZones();
	// TD17456

private:

    AbstractStationBroadcast( const AbstractStationBroadcast& theAbstractStationBroadcast);   

protected:

    CORBA::Octet                                    m_announceId;
    bool                                            m_wasExecutedSuccessfully;

};


////////////////////////
//      INLINES
////////////////////////




inline CORBA::Octet AbstractStationBroadcast::getAnnounceId()
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );
    return m_announceId;
}

inline void AbstractStationBroadcast::setAnnounceId(CORBA::Octet announceId, bool sendSynchronisation)
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );
    m_announceId = announceId;

    if (sendSynchronisation)
    {        
        CORBA::Any data;
        data <<= CORBA::Any::from_octet(announceId);

        PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
            (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_STATION_ANNOUNCE_ID, data, getBroadcastId().c_str());
    }

}

inline void AbstractStationBroadcast::setWasExecutedSuccessfully(bool value, bool sendSynchronisation)
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    m_wasExecutedSuccessfully = value;

    if (sendSynchronisation)
    {
        CORBA::Any data;
        data <<= CORBA::Any::from_boolean(value);

        PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
            (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_WAS_EXECUTED_SUCCESSFULLY, data, getBroadcastId().c_str());
    }
}

inline bool AbstractStationBroadcast::getWasExecutedSuccessfully()
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    return m_wasExecutedSuccessfully;
}

inline TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* AbstractStationBroadcast::getBroadcastParametersCopy()
{
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* parameters = 
        AbstractBroadcast::getBroadcastParametersCopy();

    int originalLength = parameters->length();
    parameters->length(originalLength+2);

    CORBA::Any data1;
    data1 <<= CORBA::Any::from_octet(getAnnounceId());
    (*parameters)[originalLength].broadcastId = getBroadcastId().c_str();
    (*parameters)[originalLength].parameter   = TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_STATION_ANNOUNCE_ID;
    (*parameters)[originalLength].data        = data1;

    CORBA::Any data2;
    data2 <<= CORBA::Any::from_boolean(getWasExecutedSuccessfully());
    (*parameters)[originalLength+1].broadcastId = getBroadcastId().c_str();
    (*parameters)[originalLength+1].parameter   = TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_WAS_EXECUTED_SUCCESSFULLY;
    (*parameters)[originalLength+1].data        = data2;


    return parameters;
}


inline void AbstractStationBroadcast::setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter )
{
    if (parameter.parameter == TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_STATION_ANNOUNCE_ID)
    {
        CORBA::Octet updateData = 0;
        
        if ( 0 != (parameter.data >>= CORBA::Any::to_octet(updateData) ))
        {
            setAnnounceId(updateData, false);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                "BroadcastParameter update received: AnnounceId=%d", updateData );
        }
    }
    else if (parameter.parameter == TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_WAS_EXECUTED_SUCCESSFULLY)
    {
        bool updateData = 0;
        
        if ( 0 != (parameter.data >>= CORBA::Any::to_boolean(updateData) ))
        {
            setWasExecutedSuccessfully(updateData, false);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                "BroadcastParameter update received: WasExecutedSuccessfully=%s", updateData?"true":"false");
        }
    }
    else
    {
        AbstractBroadcast::setBroadcastParameter(parameter);
    }
}

} // namespace TA_IRS_App

#endif

