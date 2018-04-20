/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/AbstractStationBroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */


#if !defined(AbstractStationBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define AbstractStationBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "app/pa/PAAgent/src/AbstractBroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"

namespace TA_IRS_App
{

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

