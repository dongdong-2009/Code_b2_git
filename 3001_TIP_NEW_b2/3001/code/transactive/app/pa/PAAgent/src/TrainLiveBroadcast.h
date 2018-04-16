/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/TrainLiveBroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2012/11/29 22:02:45 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#if !defined(TrainLiveBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define TrainLiveBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/AbstractTrainBroadcast.h"


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


        virtual void validateBroadcastConfig(); //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException);

        virtual void prepare();
        virtual void execute();
        virtual void aborting();

        virtual void terminate();

        virtual TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage getTrainCoverage();

        virtual TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices getTrainsRequiringContinue();

        virtual void setHasOverrideOption(bool override);

        void beginTrainLiveAnnouncement();

        void continueTrainLiveAnnouncement();

        void endTrainLiveAnnouncement();

        virtual void processRuntimeNotification( const TA_IRS_Bus::PaTypes::PaUpdateEvent& event );

        virtual void processRuntimeNotification( const TA_IRS_Bus::PaTypes::PaAnnouncementUpdate& event );

    protected:

        virtual void runtimeSpecifics();

    private:

        TrainLiveBroadcast( const TrainLiveBroadcast& theTrainLiveBroadcast);

        // the train agent sends an asynchronous command result notification back
        void handleCommandResult( TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices& seqCoverage,
            unsigned int trainIndexIndex, const TA_IRS_Bus::PaTypes::PaUpdateEvent& event );

        bool areAllTrainsReset();

        bool areAllTrainsFinished();

        void disconnect();

        // this is used to indicate the next logical state of the broadcast
        // ie if the action was setup, then the next logical state is train ready
        // if the action was end, then the next logical state is completed.
        TA_Base_Bus::IPAAgentCorbaDef::ELocalState m_nextState;

    protected:

        TA_Base_Bus::IPAAgentCorbaDef::TrainLiveConfig& m_theSpecifics; // For convenience
    };


} // namespace TA_IRS_App

#endif

