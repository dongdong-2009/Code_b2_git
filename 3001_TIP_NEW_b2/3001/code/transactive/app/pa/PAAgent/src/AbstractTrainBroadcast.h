/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/AbstractTrainBroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/11/29 22:02:45 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#if !defined(AbstractTrainBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define AbstractTrainBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "ace/RW_Mutex.h"

#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"
#pragma warning ( disable : 4250 )
#include "bus/trains/TrainCorbaProxy/src/TrainPaCorbaProxy.h"
#pragma warning ( default : 4250 )

#include "app/pa/PAAgent/src/AbstractBroadcast.h"

namespace TA_IRS_App
{

    class AbstractTrainBroadcast : public AbstractBroadcast
    {

    public:

        AbstractTrainBroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                                const std::string& sessionId);
            // throws CorruptInternalStateDataException

        virtual ~AbstractTrainBroadcast();

        virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* getBroadcastParametersCopy();
        virtual void setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter );

        virtual void retry( const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
                            CORBA::Boolean hasOverrideOption,
                            const std::string& sessionId );
            // throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException);

        virtual TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage getTrainCoverage() = 0;
        virtual void setHasOverrideOption(bool override) = 0;

        TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage   getEffectiveTrainCoverage(
            const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains );
        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices calculateEffectiveCoverageIndices(
            const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& allTrains,
            const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& newTrains );

        // Agent-synchronised BroadcastParameter
        CORBA::Octet getAnnouncementId();
        void setAnnouncementId(CORBA::Octet announcementId, bool sendSynchronisation = true);

        virtual bool getIsPartOfBroadcast(CORBA::Octet trainId, CORBA::Octet announcementId) = 0;

        virtual void processRuntimeNotification( const TA_IRS_Bus::PaTypes::PaUpdateEvent& event ) = 0;

        CORBA::Octet getNextAvailableTrainAnnouncementId();
            //throw(TA_Base_Core::NoFreeAnnounceIdException);
        void releaseUsingAnnouncementId( const CORBA::Octet cUsingId );
        void occupyUsingAnnouncementId( const CORBA::Octet cUsingId );

    private:

        AbstractTrainBroadcast( const AbstractTrainBroadcast& theAbstractTrainBroadcast);

    protected:
        // to prevent the broadcast finishing while waiting for train statuses to return
        time_t m_terminateTimeLimit;

        TA_IRS_Bus::TrainPaCorbaProxyPtr                m_trainPaCorbaProxy;
        CORBA::Octet                                    m_trainAnnouncementId;

        static ACE_RW_Mutex s_mtxUsedAnnouncementId;
        static std::set<CORBA::Octet> s_setUsedAnnouncementIds;
    };

} // namespace TA_IRS_App

#endif

