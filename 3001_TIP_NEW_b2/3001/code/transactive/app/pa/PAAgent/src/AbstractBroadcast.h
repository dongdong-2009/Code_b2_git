/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/AbstractBroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2013/09/11 15:41:59 $
  * Last modified by:  $Author: huangjian $
  *
  */

#ifndef ABSTRACTBROADCAST_INCLUDED_
#define ABSTRACTBROADCAST_INCLUDED_

#include <string>
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/exceptions/src/PasExceptions.h"
#pragma warning ( disable : 4250 )
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#pragma warning ( default : 4250 )
#include "app/pa/PAAgent/src/ProgressReportSentry.h"
#include "app/pa/PAAgent/src/PasHelpers.h"

namespace TA_IRS_App
{

class AbstractBroadcast
{

public:

    AbstractBroadcast(TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId);
        // throws CorruptInternalStateDataException

    void setProgressReport(const TA_Base_Bus::IPAAgentCorbaDef::ProgressReport& progressReport)
    {
		if(NULL == m_progressReportSentry)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_progressReportSentry is NULL" );
			return;
		}
        m_progressReportSentry->setProgressReport(progressReport);
    }

    virtual ~AbstractBroadcast();

    virtual void validateBroadcastConfig() = 0;

    virtual void operatorAbort();
    virtual void prepare() = 0;
    virtual void execute() = 0;
    virtual void terminate() = 0;
    virtual void aborting() = 0;
    virtual bool isBusy();
    void updateRuntimeState();  // calls runtimeSpecifics()

    virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* getBroadcastConfigCopy();

    virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* getBroadcastParametersCopy();
    virtual void setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter );
        // throws CorruptInternalStateDataException

    std::string getBroadcastId();
    void setBroadcastId(const std::string& broadcastId);

    TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType getBroadcastType();

    bool getIsEventTriggered();
        
    time_t getLastUpdate();

    ///////////////////////////////////////////////////////////////////////////
    // Delegation to ProgressReportSentry
    ///////////////////////////////////////////////////////////////////////////

    TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState getBroadcastState()
    {
		if(NULL == m_progressReportSentry)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_progressReportSentry is NULL" );
			return TA_Base_Bus::IPAAgentCorbaDef::Termination;
		}
        return m_progressReportSentry->getBroadcastState();    
    }
    void setBroadcastState (TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState state)
    {
		if(NULL == m_progressReportSentry)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_progressReportSentry is NULL" );
			return;
		}
        m_progressReportSentry->setBroadcastState(state);
        resetRetries();
    }

    TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* getProgressReportCopy()
    {
		if(NULL == m_progressReportSentry)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_progressReportSentry is NULL" );
			return NULL;
		}
        return m_progressReportSentry->getProgressReportCopy();
    }

    bool getHasProgressReportChanged()
    {
		if(NULL == m_progressReportSentry)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_progressReportSentry is NULL" );
			return false;
		}
        return m_progressReportSentry->getHasProgressReportChanged();
    }

    void setHasProgressReportChanged(bool changed)
    {
		if(NULL == m_progressReportSentry)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_progressReportSentry is NULL" );
			return;
		}
        m_progressReportSentry->setHasProgressReportChanged(changed);
    }

    // Agent-synchronised BroadcastParameter
    std::string getSessionId();
    void setSessionId(const std::string& sessionId, bool sendSynchronisation = true);

    // Agent-synchronised BroadcastParameter
    bool getIsTaggedForRemoval();
    void setIsTaggedForRemoval(bool isTaggedForRemoval, bool sendSynchronisation = true);

    // Agent-synchronised BroadcastParameter
    bool getIsTaggedForAborted();
    void setIsTaggedForAborted(bool isTaggedForAborted, bool sendSynchronisation = true);

    // Agent-synchronised BroadcastParameter
    TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices getEffectiveCoverageIndices( );
    void setEffectiveCoverageIndices( const TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices& indicies, bool sendSynchronisation = true );

    void resetRetries();
    void determineWhetherToRetryOrAbort(TA_Base_Bus::IPAAgentCorbaDef::EGlobalState fault);

    virtual void updateFinalProgressReport( const bool bAbortedIsError = true );

    bool getWasExecutedSuccessfully();
    void setWasExecutedSuccessfully(const bool bExecuted, bool sendSynchronisation = true);   

protected:

    virtual void runtimeSpecifics() = 0;

    void abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::EGlobalState fault);
    void abnormalContinuation(TA_Base_Bus::IPAAgentCorbaDef::EGlobalState fault);
	// TD17456
	void noneValidZoneTermination(TA_Base_Bus::IPAAgentCorbaDef::EGlobalState fault);
    // TD17456
    void linklostTermination(TA_Base_Bus::IPAAgentCorbaDef::EGlobalState fault);


private:

    AbstractBroadcast( const AbstractBroadcast& theAbstractBroadcast);
    

protected:

    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig_var m_broadcastConfigV;
    ProgressReportSentry* m_progressReportSentry;

    // Usual lock for runtime operations
    TA_Base_Core::ReEntrantThreadLockable m_lock;

    // Special lock to limit locked time for accesses to global broadcast information 
    // frequently accessed by AbstractCircularList e.g. announcementIds, broadcastIds
    // broadcastTypes ...
    TA_Base_Core::ReEntrantThreadLockable m_lockForBroadcastParameters;

    time_t m_lastUpdate;
    bool m_hasProgressReportChanged;
    bool m_isTaggedForRemoval;
    TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices m_effectiveCoverageIndices;
    int m_retries;
    static const int MAX_NON_FATAL_RETRIES;
    bool m_wasExecutedSuccessfully;
    bool m_bTaggedForAborted;
};



////////////////////////
//      INLINES
////////////////////////


inline void AbstractBroadcast::setWasExecutedSuccessfully(const bool bExecuted, bool sendSynchronisation)
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    if ( bExecuted == m_wasExecutedSuccessfully )
    {
        return;
    }

    m_wasExecutedSuccessfully = bExecuted;

    if (sendSynchronisation)
    {
        CORBA::Any data;
        data <<= CORBA::Any::from_boolean(bExecuted);

        PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
            (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_WAS_EXECUTED_SUCCESSFULLY, data, getBroadcastId().c_str());
    }
}

inline bool AbstractBroadcast::getWasExecutedSuccessfully()
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    return m_wasExecutedSuccessfully;
}


inline std::string AbstractBroadcast::getSessionId()
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    // For Solaris 
    return m_broadcastConfigV->sessionId.in();
}

inline void AbstractBroadcast::setSessionId(const std::string& sessionId, bool sendSynchronisation)
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    // Note CORBA::String_members will automatically free and reallocate memory for const char* assignment
    m_broadcastConfigV->sessionId = sessionId.c_str();

    if (sendSynchronisation)
    {
        CORBA::Any data;
        data <<= CORBA::Any::from_string(sessionId.c_str(),0);

        PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
            (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_SESSION_ID, data, getBroadcastId().c_str());
    }
}

inline bool AbstractBroadcast::getIsTaggedForRemoval()
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    return m_isTaggedForRemoval;
}

inline void AbstractBroadcast::setIsTaggedForRemoval(bool isTaggedForRemoval, bool sendSynchronisation)
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    m_isTaggedForRemoval = isTaggedForRemoval;

    if (sendSynchronisation)
    {
        CORBA::Any data;
        data <<= CORBA::Any::from_boolean(isTaggedForRemoval);

        PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
            (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_IS_TAGGED_FOR_REMOVAL, data, getBroadcastId().c_str());
    }
}

inline bool AbstractBroadcast::getIsTaggedForAborted()
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    return m_bTaggedForAborted;
}

inline void AbstractBroadcast::setIsTaggedForAborted(bool bAborted, bool sendSynchronisation)
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    if ( m_bTaggedForAborted == bAborted )
    {
        return;
    }

    m_bTaggedForAborted = bAborted;

    if (sendSynchronisation)
    {
        CORBA::Any data;
        data <<= CORBA::Any::from_boolean(bAborted);

        PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
            (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_OPERATOR_ABORTED, data, getBroadcastId().c_str());
    }
}

inline TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices AbstractBroadcast::getEffectiveCoverageIndices()
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    return m_effectiveCoverageIndices;
}

inline void AbstractBroadcast::setEffectiveCoverageIndices(const TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices& indicies, bool sendSynchronisation)
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    m_effectiveCoverageIndices = indicies;

    if (sendSynchronisation)
    {
        CORBA::Any data;
        data <<= indicies;

        PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
            (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_EFFECTIVE_COVERAGE_INDICES, data, getBroadcastId().c_str());
    }
}

inline TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* AbstractBroadcast::getBroadcastParametersCopy()
{
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* parameters = 
        new TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq;

    parameters->length(5);

    CORBA::Any data1;
    data1 <<= CORBA::Any::from_string(getSessionId().c_str(),0);
    (*parameters)[0].broadcastId = getBroadcastId().c_str();
    (*parameters)[0].parameter   = TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_SESSION_ID;
    (*parameters)[0].data        = data1;

    CORBA::Any data2;
    data2 <<= CORBA::Any::from_boolean(getIsTaggedForRemoval());
    (*parameters)[1].broadcastId = getBroadcastId().c_str();
    (*parameters)[1].parameter   = TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_IS_TAGGED_FOR_REMOVAL;
    (*parameters)[1].data        = data2;

    CORBA::Any data3;
    data3 <<= getEffectiveCoverageIndices();
    (*parameters)[2].broadcastId = getBroadcastId().c_str();
    (*parameters)[2].parameter   = TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_EFFECTIVE_COVERAGE_INDICES;
    (*parameters)[2].data        = data3;

    CORBA::Any data4;
    data3 <<= CORBA::Any::from_boolean(getIsTaggedForAborted());
    (*parameters)[3].broadcastId = getBroadcastId().c_str();
    (*parameters)[3].parameter   = TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_OPERATOR_ABORTED;
    (*parameters)[3].data        = data4;

    CORBA::Any data5;
    data1 <<= CORBA::Any::from_boolean(getWasExecutedSuccessfully());
    (*parameters)[4].broadcastId = getBroadcastId().c_str();
    (*parameters)[4].parameter   = TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_WAS_EXECUTED_SUCCESSFULLY;
    (*parameters)[4].data        = data5;

    return parameters;
}

inline void AbstractBroadcast::setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter )
{

    if (parameter.parameter == TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_WAS_EXECUTED_SUCCESSFULLY)
    {
        bool updateData = false;

        if ( 0 != (parameter.data >>= CORBA::Any::to_boolean(updateData) ))
        {
            setWasExecutedSuccessfully(updateData, false);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                "BroadcastParameter update received: WasExecutedSuccessfully=%s", updateData?"true":"false");
        }
    }
    else if (parameter.parameter == TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_SESSION_ID)
    {
        const char* updateData = NULL;
        
        if ( 0 != (parameter.data >>= CORBA::Any::to_string(updateData, 0) ))
        {
            setSessionId(updateData, false);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                "BroadcastParameter update received: SessionId=%s", updateData );
        }
    }
    else if (parameter.parameter == TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_IS_TAGGED_FOR_REMOVAL)
    {
        bool updateData = false;
        
        if ( 0 != (parameter.data >>= CORBA::Any::to_boolean(updateData) ))
        {
            setIsTaggedForRemoval(updateData, false);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                "BroadcastParameter update received: IsTaggedForRemoval=%s", updateData?"true":"false");
        }
    }
    else if (parameter.parameter == TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_EFFECTIVE_COVERAGE_INDICES)
    {
        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices* updateData = 0;
        
        if ( 0 != (parameter.data >>= updateData ))
        {
            setEffectiveCoverageIndices(*updateData, false);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                "BroadcastParameter update received: EffectiveCoverageIndices");
        }
    }
    else if (parameter.parameter == TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_OPERATOR_ABORTED)
    {
        bool updateData = false;

        if ( 0 != (parameter.data >>= CORBA::Any::to_boolean(updateData) ))
        {
            setIsTaggedForAborted(updateData, false);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                "BroadcastParameter update received: IsOperatorAborted=%s", updateData?"true":"false");
        }
    }
    else
    {
        // Should never get here
        TA_THROW(TA_Base_Core::CorruptInternalStateDataException("Unrecognised BroadcastParameter update received"));
    }
}

} // namespace TA_IRS_App

#endif // ABSTRACTBROADCAST_INCLUDED_

