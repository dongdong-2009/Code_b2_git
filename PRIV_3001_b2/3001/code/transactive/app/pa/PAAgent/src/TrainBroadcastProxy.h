#ifndef TRAINBROADCASTPROXY_H
#define TRAINBROADCASTPROXY_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/TrainBroadcastProxy.h $
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
#include "bus/pa_4669/pa_agent/IDL/src/ITrainPaMethod.h"

namespace TA_IRS_App
{

class BroadcastManager;
class AbstractBroadcast;

/**
 * This class manages the state of broadcasts within the PA Agent.
 */
class TrainBroadcastProxy : public virtual POA_TA_Base_Bus::ITrainPaMethod,
    public virtual TA_Base_Core::ServantBase
{
public:
    TrainBroadcastProxy( BroadcastManager& refManager );
    virtual ~TrainBroadcastProxy();

    // IDL Interface functions
    virtual char* broadcastTrainDva(const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains, CORBA::ULong messageKey, 
        CORBA::ULong libraryVersion, const TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams& stCyclicParams, 
        CORBA::Boolean hasOverrideOption, const char* sessionId, CORBA::Boolean isEmergency);
    virtual char* broadcastTrainLive(const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains, 
        CORBA::Boolean hasOverrideOption, const char* staticGroup, const char* sessionId);
    virtual void beginTrainLiveAnnouncement(const char* broadcastId);
    virtual void continueTrainLiveAnnouncement(const char* broadcastId);
    virtual void endTrainLiveAnnouncement(const char* broadcastId);
    virtual void retryTrainBroadcast(const char* broadcastId, const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains, 
        CORBA::Boolean hasOverrideOption, const char* sessionId);
    virtual void terminateBroadcast(const char* broadcastId, const char* sessionId);
    virtual void removeBroadcast(const char* broadcastId, const char* sessionId);
    virtual void restoreBroadcast(const char* broadcastId, const char* sessionId);
    virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* getBroadcastIdTypes();
    virtual void changeBroadcastId(const char* fromBroadcastId, const char* toBroadcastId, const char* sessionId);
    virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* getBroadcastConfig(const char* broadcastId);
    virtual TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* getProgressReport(const char* broadcastId);
    virtual TA_Base_Bus::IPAAgentCorbaDef::TrainDvaMessageRecords* getTrainDvaMessageRecords();

    virtual void broadcastEventTriggeredTrainDva(CORBA::ULong messageKey, CORBA::Short libraryVersion, const TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams& announcementPeriod, const char* sessionId, const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains);
    virtual void broadcastEventTriggeredTrainDvaForStation(CORBA::ULong messageKey, CORBA::Short libraryVersion, const TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams& announcementPeriod, const char* sessionId);
    virtual void broadcastEventTriggeredTrainDvaForInbound(CORBA::ULong messageKey, CORBA::Short libraryVersion, const TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams& announcementPeriod, const char* sessionId);
    virtual void broadcastEventTriggeredTrainDvaForOutbound(CORBA::ULong messageKey, CORBA::Short libraryVersion, const TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams& announcementPeriod, const char* sessionId);

public:

    static const std::string ATS_AGENT_SESSION_ID;
    static const std::string MANAGER_LAUNCHING_BROADCAST_ID;

protected:
    std::string getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType broadcastType,
                                  const std::string& extraInformation = "");
        // TA_THROW CorruptInternalStateDataException

    BroadcastManager& m_refManager;
    TA_Base_Core::ReEntrantThreadLockable m_lock;
};

} // namespace TA_IRS_App

#pragma warning (default : 4250 )

#endif // TRAINBROADCASTPROXY_H

