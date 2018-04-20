/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/AbstractNonTrainDVABroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */


#if !defined(AbstractNonTrainDVABroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define AbstractNonTrainDVABroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "app/pa/PAAgent/src/AbstractStationBroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/Table200.h"
#include "app/pa/PAAgent/src/Table202.h"
// jeffrey++ Bug457
#include "app/pa/PAAgent/src/Table302.h"
// ++jeffrey Bug457

// ++ Bug456
#include "core/utilities/src/Timer.h"
// ++ Bug456

namespace TA_IRS_App
{

class Table301;
class Table302;
class Table350;


class AbstractNonTrainDVABroadcast : public virtual AbstractStationBroadcast
{

public:

    AbstractNonTrainDVABroadcast( Table202& table202,
                                  Table302& table302,
                                  Table350& table350,
                                  TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
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

	//Maochun Sun++
	//TD13411
    //virtual void updateCacheWithLatestZoneStatus(unsigned long zoneKey)=0;
	virtual void updateCacheWithLatestZoneStatus()=0;
        // throws (PasConnectionException)

	//++Maochun Sun
	//TD13411

    virtual void sendRequestToTisAgent(const TA_IRS_Bus::StationDvaMessageRecord& record, 
                                        const char* startTimeString,
                                        const char* stopTimeString,
                                        CORBA::UShort tisPriority)=0;
        // throws (...)


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

	// jeffrey++ Bug457
	virtual int getZoneStatusTableIndex();
	// ++jeffrey Bug457

protected:
  
    virtual void runtimeSpecifics();
    bool isCyclic();

    virtual void setupAndValidateTisParameters(const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones)=0;
        //throw(TA_Base_Core::BadParameterException);

    void executeTisBroadcast();

    
private:

    AbstractNonTrainDVABroadcast( const AbstractNonTrainDVABroadcast& theAbstractNonTrainDVABroadcast);    
	// ++ Bug456
    void executePhase1();
    void executePhase2();
	bool m_timerStarted;
private:
    TA_Base_Bus::IPAAgentCorbaDef::EExecutePhase m_executePhase;
    TA_Base_Core::Timer m_executePhaseTime;
	// ++ Bug456    

protected:

    typedef std::map<unsigned long, TA_IRS_App::ZoneStatus>     ZoneKeyToZoneStatusMap;

    static const std::string                        EXPECTED_STATION_SYSTEM;

    TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig&     m_theSpecifics; // For convenience

    TA_IRS_App::Table302&                                       m_table302;
    TA_IRS_App::Table202&                                       m_table202;
    TA_IRS_App::Table350&                                       m_table350;
    
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

