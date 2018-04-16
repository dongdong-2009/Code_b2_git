/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/AbstractTrainBroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//
//  AbstractDecisionEngine.h
//  Implementation of the Class AbstractDecisionEngine
//  Generated by Enterprise Architect
//  Created on:      25-Sep-2003 18:53:34
//  Original author: 
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////


#if !defined(AbstractTrainBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define AbstractTrainBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/AbstractBroadcast.h"

#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"
#include "bus/trains/TrainCorbaProxy/src/TrainPaCorbaProxy.h"

#include "core/naming/src/FlexibleTimeoutNamedObject.h"

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
            // throw(TA_Base_Core::BadParameterException);

        virtual void updateFinalProgressReport();


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


    private:

        AbstractTrainBroadcast( const AbstractTrainBroadcast& theAbstractTrainBroadcast);

    protected:

        TA_IRS_Bus::TrainPaCorbaProxyPtr                m_trainPaCorbaProxy;
        CORBA::Octet                                    m_trainAnnouncementId;
    };


    ////////////////////////
    //      INLINES
    ////////////////////////


    inline CORBA::Octet AbstractTrainBroadcast::getAnnouncementId()
    {
        FUNCTION_ENTRY( "getAnnouncementId" );

        TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

        FUNCTION_EXIT;
        return m_trainAnnouncementId;
    }


    inline void AbstractTrainBroadcast::setAnnouncementId(CORBA::Octet announcementId, bool sendSynchronisation)
    {
        FUNCTION_ENTRY( "setAnnouncementId" );

        TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters);

        m_trainAnnouncementId = announcementId;

        if (sendSynchronisation)
        {
            CORBA::Any data;
            data <<= CORBA::Any::from_octet(announcementId);

            PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
                (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_TRAIN_ANNOUNCEMENT_ID, data, getBroadcastId().c_str());
        }

        FUNCTION_EXIT;
    }


    inline TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* AbstractTrainBroadcast::getBroadcastParametersCopy()
    {
        FUNCTION_ENTRY( "getBroadcastParametersCopy" );

        TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* parameters =
            AbstractBroadcast::getBroadcastParametersCopy();

        int originalLength = parameters->length();
        parameters->length(originalLength+1);

        CORBA::Any data1;
        data1 <<= CORBA::Any::from_octet(getAnnouncementId());
        (*parameters)[originalLength].broadcastId = getBroadcastId().c_str();
        (*parameters)[originalLength].parameter   = TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_TRAIN_ANNOUNCEMENT_ID;
        (*parameters)[originalLength].data        = data1;

        FUNCTION_EXIT;
        return parameters;
    }


    inline void AbstractTrainBroadcast::setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter )
    {
        FUNCTION_ENTRY( "setBroadcastParameter" );

        if (parameter.parameter == TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_TRAIN_ANNOUNCEMENT_ID)
        {
            CORBA::Octet announcementId;

            if ( 0 != (parameter.data >>= CORBA::Any::to_octet(announcementId) ))
            {
                setAnnouncementId(announcementId, false);
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                    "BroadcastParameter update received: announcementId=%d", announcementId );
            }
        }
        else
        {
            AbstractBroadcast::setBroadcastParameter(parameter);
        }

        FUNCTION_EXIT;
    }


} // namespace TA_IRS_App

#endif

