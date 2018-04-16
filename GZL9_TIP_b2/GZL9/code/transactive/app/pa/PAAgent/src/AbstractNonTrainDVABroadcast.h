/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/GZL9_TIP/GZL9/transactive/app/pa/PAAgent/src/AbstractNonTrainDVABroadcast.cpp $
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */


#if !defined(ABSTRACTNONTRAINDVABROADCAST__INCLUDED_)
#define ABSTRACTNONTRAINDVABROADCAST__INCLUDED_

#include "core/data_access_interface/tis_agent/IDL/src/TISMessageDataDefinitions.h"
#include "app/pa/PAAgent/src/AbstractStationBroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"

#include "core/utilities/src/Timer.h"


namespace TA_IRS_App
{

class AbstractNonTrainDVABroadcast : public virtual AbstractStationBroadcast
{

public:

    AbstractNonTrainDVABroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                                  const std::string& sessionId);
    virtual ~AbstractNonTrainDVABroadcast();

    virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* getBroadcastParametersCopy();
    virtual void setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter );

    virtual void validateBroadcastConfig(); //throw(TA_Base_Core::BadParameterException);
    virtual void validateBroadcastConfigSpecifics()=0; //throw(TA_Base_Core::BadParameterException);

    virtual void execute();

    virtual void writeToMessagesSequenceBroadcastRequestTable(unsigned int announceId)=0;
        // throws (PasWriteErrorException, PasConnectionException)

    virtual void performVersionCheck()
    {}

    virtual void disconnectSourceId() = 0;
    virtual void disconnectSequenceId() = 0;

    void updateCurrentlyPlayingZones();
    void updateAllZones(TA_Base_Bus::IPAAgentCorbaDef::ELocalState localState);

    //virtual void updateCacheWithLatestZoneStatus(unsigned long zoneKey)=0;
	virtual void updateCacheWithLatestZoneStatus()=0;
        // throws (PasConnectionException)

    virtual void terminate();

    virtual TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage getZoneCoverage()
    {
        TA_Base_Core::ThreadGuard guard( m_lock );
        return m_theSpecifics.zones;
    }
    
    // Agent-synchronised BroadcastParameter
    CORBA::Octet getMessageSeqId();
    void setMessageSeqId(CORBA::Octet value, bool sendSynchronisation = true);


    virtual void updateFinalProgressReport();

	virtual int getZoneStatusTableIndex();

protected:
  
    virtual void runtimeSpecifics();
    bool isCyclic();

private:

    AbstractNonTrainDVABroadcast( const AbstractNonTrainDVABroadcast& theAbstractNonTrainDVABroadcast);    
	bool m_timerStarted;

protected:

    typedef std::map<unsigned long, TA_IRS_App::ZoneStatus>     ZoneKeyToZoneStatusMap;

    static const std::string                        EXPECTED_STATION_SYSTEM;

    TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig&     m_theSpecifics; // For convenience

    TA_Base_Core::STISDestination                        m_tisCoverage;
    
    CORBA::Octet                                    m_messageSeqId;

    ZoneKeyToZoneStatusMap                          m_lastZoneStatuses;

	// jeffrey++ Bug457
    ZoneKeyToZoneStatusMap m_previouslastZoneStatuses;
	MessageSequenceStatus m_previousmsgSeqStatus;
	int m_zoneStatusTableIndex;
	// ++jeffrey Bug457
};




////////////////////////
//      INLINES
////////////////////////


inline CORBA::Octet AbstractNonTrainDVABroadcast::getMessageSeqId()
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );
    return m_messageSeqId;
}

inline void AbstractNonTrainDVABroadcast::setMessageSeqId(CORBA::Octet value, bool sendSynchronisation)
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );
    m_messageSeqId = value;

    if (sendSynchronisation)
    {
        CORBA::Any data;
        data <<= CORBA::Any::from_octet(value);

        PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
            (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_MESSAGE_SEQUENCE_ID, data, getBroadcastId().c_str());
    }
}



inline TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* AbstractNonTrainDVABroadcast::getBroadcastParametersCopy()
{
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* parameters = 
        AbstractStationBroadcast::getBroadcastParametersCopy();

    int originalLength = parameters->length();
    parameters->length(originalLength+1);

    CORBA::Any data1;
    data1 <<= CORBA::Any::from_octet(getMessageSeqId());
    (*parameters)[originalLength].broadcastId = getBroadcastId().c_str();
    (*parameters)[originalLength].parameter   = TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_MESSAGE_SEQUENCE_ID;
    (*parameters)[originalLength].data        = data1;


    return parameters;
}

inline void AbstractNonTrainDVABroadcast::setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter )
{
    if (parameter.parameter == TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_MESSAGE_SEQUENCE_ID)
    {
        CORBA::Octet updateData = 0;
        
        if ( 0 != (parameter.data >>= CORBA::Any::to_octet(updateData) ))
        {
            setMessageSeqId(updateData, false);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                "BroadcastParameter update received: MessageSeqId=%d", updateData );
        }
    }
    else
    {
        AbstractStationBroadcast::setBroadcastParameter(parameter);
    }
}

} // namespace TA_IRS_App

#endif

