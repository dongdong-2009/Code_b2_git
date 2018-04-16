/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/ProgressReportSentry.h $
  * @author:  Ripple
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2012/11/29 22:02:45 $
  * Last modified by:  $Author: qi.huang $
  *
  */


#if !defined(ProgressReportSentry_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define ProgressReportSentry_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#pragma warning ( disable : 4250 )
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#pragma warning ( default : 4250 )
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_IRS_App
{

class ProgressReportSentry
{

public:

    ProgressReportSentry( const CORBA::String_member& broadcastId, 
                          const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones);
    
    ProgressReportSentry( const CORBA::String_member& broadcastId, 
                          const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains);

    virtual ~ProgressReportSentry();

    void setProgressReport(const TA_Base_Bus::IPAAgentCorbaDef::ProgressReport& progressReport);

    TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* getProgressReportCopy();// Proper copy

    void setBroadcastId(const std::string& broadcastId);

    TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState getBroadcastState();
    void setBroadcastState (TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState state);

    void setGlobalState (TA_Base_Bus::IPAAgentCorbaDef::EGlobalState globalState);

    void setLocalState( unsigned int index,  TA_Base_Bus::IPAAgentCorbaDef::ELocalState localState );
    TA_Base_Bus::IPAAgentCorbaDef::ELocalState getLocalState( unsigned int index );
    
    void setOverridingSourcePriority( unsigned int index, CORBA::Octet overridingSourcePriority );

    void setErrorOccurred( const unsigned int index, const bool bError );
    bool getErrorOccurred( const unsigned int index );

    void resetZoneOrTrain(unsigned int index, 
        TA_Base_Bus::IPAAgentCorbaDef::ELocalState eStaet = TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS);

    void flagNonCompletedWithErrors();
    // DTL-340 modify start
    //void flagNonCompletedWithErrorsAndAborted();
    void flagNonCompletedWithErrorsAndAborted( const bool bError = true );
    // DTL-340 modify end
	// TD17456
	void flagNonValidWithFailure();
    // TD17456

    void flagLinkLostWithErrors( const bool bError = true );

    void resetGlobalState();

    bool getHasProgressReportChanged()
    {
        // Implicitly threadsafe
        return m_hasProgressReportChanged;
    }

    void setHasProgressReportChanged(bool changed)
    {
        // Implicitly threadsafe
        m_hasProgressReportChanged = changed;
    }

    /** 
      * setInhibitAuditMessages
      *
      * For broadcasts such as playback, inhibit the sending of audit
      * messages for broadcast state updates.
      * By default audit messages are sent.
      *
      * @param inhibitAuditMessages true if no audit messages should be sent.
      */
    inline void setInhibitAuditMessages(bool inhibitAuditMessages)
    {
        m_inhibitAuditMessages = inhibitAuditMessages;
    }

protected:

    void submitBroadcastStateAuditMessage( const TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState& newState, 
                                           const std::string& broadcastId ) const;

    void submitBroadcastStateFailureAuditMessage( const TA_Base_Bus::IPAAgentCorbaDef::EGlobalState& newStateFailure, 
                                                  const std::string& broadcastId ) const;

    void submitZoneBroadcastFailureAuditMessage( unsigned long index,
                                                 TA_Base_Bus::IPAAgentCorbaDef::ELocalState localState);

private:
    ProgressReportSentry( const ProgressReportSentry& theProgressReportSentry);

protected:

    TA_Base_Bus::IPAAgentCorbaDef::ProgressReport    m_progressReport;
    TA_Base_Core::NonReEntrantThreadLockable         m_lock;
    bool                                        m_hasProgressReportChanged;
    bool                                        m_inhibitAuditMessages;
    // set depending on whether the constructor is called with trains or zones
    bool                                        m_isStationBroadcast;
};

}

#endif
