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

#if !defined(TRAINLIVEBROADCAST__INCLUDED_)
#define TRAINLIVEBROADCAST__INCLUDED_

#include "app/pa/PAAgent/src/AbstractTrainBroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"

#include "bus/pa/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"

#include "core/synchronisation/src/ThreadGuard.h"


namespace TA_IRS_App
{


    class TrainLiveBroadcast : public virtual AbstractTrainBroadcast
    {

    public:

        TrainLiveBroadcast(TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                           const std::string& sessionId);
            // throws CorruptInternalStateDataException

        virtual ~TrainLiveBroadcast();

        virtual bool getIsPartOfBroadcast(CORBA::Octet trainId, CORBA::Octet announcementId);
        int getIndexForTrain(CORBA::Octet trainId);


        virtual void validateBroadcastConfig(); //throw(TA_Base_Core::BadParameterException);

        virtual void execute();

        virtual void terminate();

        virtual TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage getTrainCoverage()
        {
            TA_Base_Core::ThreadGuard guard( m_lock );
            return m_theSpecifics.trains;
        }


        virtual TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage getTrainsRequiringContinue();


        virtual void setHasOverrideOption(bool override)
        {
            TA_Base_Core::ThreadGuard guard( m_lock );
            m_theSpecifics.hasOverrideOption = override;
        }

        void beginTrainLiveAnnouncement();

        void continueTrainLiveAnnouncement();

        void endTrainLiveAnnouncement();

        virtual void processRuntimeNotification( const TA_IRS_Bus::PaTypes::PaUpdateEvent& event );

        virtual void processRuntimeNotification( const TA_IRS_Bus::PaTypes::PaAnnouncementUpdate& event );

        virtual void retry( const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
                            CORBA::Boolean hasOverrideOption,
                            const std::string& sessionId );

    protected:

        virtual void runtimeSpecifics();

    private:

        TrainLiveBroadcast( const TrainLiveBroadcast& theTrainLiveBroadcast);

        // the train agent sends an asynchronous command result notification back
        void handleCommandResult(
            TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices& effectiveCoverageIndices,
            unsigned int trainIndexIndex,
            const TA_IRS_Bus::PaTypes::PaUpdateEvent& event );

        bool areAllTrainsReset();

        bool areAllTrainsFinished();

        void disconnect();

        // this is used to indicate the next logical state of the broadcast
        // ie if the action was setup, then the next logical state is train ready
        // if the action was end, then the next logical state is completed.
        TA_Base_Bus::IPAAgentCorbaDef::ELocalState m_nextState;

        // to prevent the broadcast finishing while waiting for train statuses to return
        time_t m_terminateTimeLimit;

    protected:

        TA_Base_Bus::IPAAgentCorbaDef::TrainLiveConfig&      m_theSpecifics; // For convenience


    };


} // namespace TA_IRS_App

#endif

