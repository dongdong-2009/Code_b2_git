#ifndef STATIONBROADCASTPROXY_H
#define STATIONBROADCASTPROXY_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/StationBroadcastProxy.h $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification: $Date: 2018/03/14 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  */


#pragma warning (disable : 4250 )

#include <string>

#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/pa_4669/pa_agent/IDL/src/IStationPaMethod.h"

namespace TA_IRS_App
{

class BroadcastManager;
class AbstractBroadcast;

/**
 * This class manages the state of broadcasts within the PA Agent.
 */
class StationBroadcastProxy : public virtual POA_TA_Base_Bus::IStationPaMethod,
    public virtual TA_Base_Core::ServantBase
{
public:
    StationBroadcastProxy( BroadcastManager& refManager );
    virtual ~StationBroadcastProxy();

    // IDL Interface functions
    virtual void agentLinkTest();
    virtual bool hardwareLinkTest(const char* broadcastId, const char* sessionId);
    virtual char* broadcastStationScheduleDva(const TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages, 
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, CORBA::UShort dwellInSecs, 
        CORBA::Boolean hasChime, CORBA::Boolean isSynchronisedWithTis, const char* groupName, 
        const TA_Base_Bus::IPAAgentCorbaDef::StationCyclicParams& stCyclicParams, const char* sessionId);
    virtual char* broadcastStationDva(const TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages, 
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, CORBA::UShort dwellInSecs, 
        CORBA::Boolean hasChime, CORBA::Boolean isSynchronisedWithTis, const char* groupName, const char* sessionId);
	virtual char* playbackStationDva(CORBA::ULong messageKey, CORBA::UShort sourceId, const char* sessionId);
    virtual char* broadcastStationLive(CORBA::Boolean hasChime, CORBA::UShort sourceId, 
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const char* groupName, const char* sessionId);
    virtual void broadcastStationMusic(TA_Base_Bus::IPAAgentCorbaDef::EMusicType noneLocalOrOcc, 
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const char* sessionId);
    virtual char* recordAdhocMessage(CORBA::UShort usSrcId, CORBA::ULong ulMsgKey, CORBA::UShort usMsgType, const char* strSession);
    virtual void retryStationBroadcast(const char* broadcastId, 
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const char* sessionId);
    virtual void terminateBroadcast(const char* broadcastId, const char* sessionId);
    virtual void removeBroadcast(const char* broadcastId, const char* sessionId);
    virtual void restoreBroadcast(const char* broadcastId, const char* sessionId);
	virtual void forceTerminateAllExistPA();
    virtual bool setAdhocType(CORBA::ULong messageKey, CORBA::UShort messageType, const char* sessionId);
    virtual bool setAdhocLabel(CORBA::ULong messageKey, const char* label, const char* sessionId);
    virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* getBroadcastIdTypes();
    virtual void changeBroadcastId(const char* fromBroadcastId, const char* toBroadcastId, const char* sessionId);
    virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* getBroadcastConfig(const char* broadcastId);
    virtual TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* getProgressReport(const char* broadcastId);
    virtual TA_Base_Bus::IPAAgentCorbaDef::StationDvaMessageRecords* getStationDvaMessageRecords();
    virtual TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords* getDvaVersionRecords();
    virtual TA_Base_Bus::IPAAgentCorbaDef::EMusicType getMusicStatus(const char* sessionId);

    virtual void broadcastEventTriggeredStationDva(CORBA::ULong messageKey, const char* messageVersionNumber, CORBA::Boolean hasChime, CORBA::Boolean isSynchronisedWithTis, CORBA::ULong durationInSecs, CORBA::UShort periodInSecs, const char* sessionId, const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones);
    virtual void broadcastEventTriggeredStationDvaForStation(CORBA::ULong messageKey, const char* messageVersionNumber, CORBA::Boolean hasChime, CORBA::Boolean isSynchronisedWithTis, CORBA::ULong durationInSecs, CORBA::UShort periodInSecs, const char* sessionId);
    virtual void broadcastEventTriggeredStationDvaForArea(CORBA::ULong messageKey, const char* messageVersionNumber, CORBA::Boolean hasChime, CORBA::Boolean isSynchronisedWithTis, CORBA::ULong durationInSecs, CORBA::UShort periodInSecs, const char* sessionId, const char* zoneArea);
    virtual void broadcastEventTriggeredStationDvaForGroup(CORBA::ULong messageKey, const char* messageVersionNumber, CORBA::Boolean hasChime, CORBA::Boolean isSynchronisedWithTis, CORBA::ULong durationInSecs, CORBA::UShort periodInSecs, const char* sessionId, const char* zoneGroupLabel);

public:

    static const std::string ATS_AGENT_SESSION_ID;

protected:

    // used by all event triggered station broadcast methods
    void broadcastEventTriggeredStationDvaInternal(
        const std::string& broadcastId,
        const CORBA::ULong& messageKey,
        const char* messageVersionNumber,
        const CORBA::Boolean& hasChime,
        const CORBA::Boolean& isSynchronisedWithTis,
        const CORBA::ULong& durationInSecs,
        const CORBA::UShort& periodInSecs,
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
        const char* sessionId );

    std::string getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType broadcastType, const std::string& extraInformation = "");
        // TA_THROW CorruptInternalStateDataException

    std::string getNewDvaBroadcastId( const unsigned long ulMsgKey, const size_t szMsgCount, const time_t tmStartTime = 0u, const time_t tmEndTime = 0u, const time_t tmInterval = 0u );

    BroadcastManager& m_refManager;
    TA_Base_Core::ReEntrantThreadLockable m_lock;
};

} // namespace TA_IRS_App

#pragma warning (default : 4250 )

#endif // STATIONBROADCASTPROXY_H

