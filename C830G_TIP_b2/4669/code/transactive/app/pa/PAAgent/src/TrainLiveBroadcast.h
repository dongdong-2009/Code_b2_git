/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/TrainLiveBroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/06/19 17:43:51 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(TrainLiveBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define TrainLiveBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/AbstractTrainBroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"

#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
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

	// Bohong++ for CL-20875, to set a special corba timeout value when doing PA train live boradcast, cos
	// the hardware may takes more than 20 seconds to make-dynamic-group when more than 3 trains.
	// Also modified PAManager & TrainAgent to consistent this timeout value, refer to PAManager & TrainAgent
	#define DGNA_TIMEOUT 80

} // namespace TA_IRS_App

#endif

