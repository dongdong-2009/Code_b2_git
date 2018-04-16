/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date: 2011-01-21 10:00:00$
  * Last modified by:  $Author: Jian Huang $
  *
  */

#include "app/trains/train_agent/src/PaTasks.h"
#include "app/trains/train_agent/src/ITimsPaManagerTaskCallback.h"
#include "app/trains/train_agent/src/IRadioGroupCoordinator.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    ContinueLiveAnnouncementTask::ContinueLiveAnnouncementTask( ITimsPaManagerTaskCallback& callback,
                                                                TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                                TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                                const std::string& sessionId,
                                                                unsigned char origin )
            : m_callback( callback ),
              m_trainId( trainId ),
              m_announcementId( announcementId ),
              m_sessionId( sessionId ),
              m_origin( origin )
    {
        FUNCTION_ENTRY( "ContinueLiveAnnouncementTask" );
        FUNCTION_EXIT;
    }


    ContinueLiveAnnouncementTask::~ContinueLiveAnnouncementTask()
    {
        FUNCTION_ENTRY( "~ContinueLiveAnnouncementTask" );
        FUNCTION_EXIT;
    }


    void ContinueLiveAnnouncementTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.continueLiveAnnouncementSingleTrain( m_trainId,
                                                        m_announcementId,
                                                        m_sessionId,
                                                        m_origin );

        FUNCTION_EXIT;
    }


    CyclicAnnouncementTask::CyclicAnnouncementTask( ITimsPaManagerTaskCallback& callback,
                                                    TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                    TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                    //unsigned char messageNumber,
                                                    unsigned short messageNumber,
                                                    //unsigned short libraryVersion,
                                                    unsigned long libraryVersion,
                                                    const TA_IRS_Bus::PaTypes::MessageSchedule& announcementPeriod,
                                                    TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode,
                                                    const std::string& sessionId,
                                                    unsigned char origin )
            : m_callback( callback ),
              m_trainId( trainId ),
              m_announcementId( announcementId ),
              m_messageNumber( messageNumber ),
              m_libraryVersion( libraryVersion ),
              m_announcementPeriod( announcementPeriod ),
              m_requestMode( requestMode ),
              m_sessionId( sessionId ),
              m_origin( origin )
    {
        FUNCTION_ENTRY( "CyclicAnnouncementTask" );
        FUNCTION_EXIT;
    }


    CyclicAnnouncementTask::~CyclicAnnouncementTask()
    {
        FUNCTION_ENTRY( "~CyclicAnnouncementTask" );
        FUNCTION_EXIT;
    }


    void CyclicAnnouncementTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.cyclicAnnouncementSingleTrain( m_trainId,
                                                  m_announcementId,
                                                  m_messageNumber,
                                                  m_libraryVersion,
                                                  m_announcementPeriod,
                                                  m_requestMode,
                                                  m_sessionId,
                                                  m_origin );

        FUNCTION_EXIT;
    }


    EndBroadcastTask::EndBroadcastTask( ITimsPaManagerTaskCallback& callback,
                                        TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                        TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                        const std::string& sessionId,
                                        unsigned char origin )
            : m_callback( callback ),
              m_trainId( trainId ),
              m_announcementId( announcementId ),
              m_sessionId( sessionId ),
              m_origin( origin )
    {
        FUNCTION_ENTRY( "EndBroadcastTask" );
        FUNCTION_EXIT;
    }


    EndBroadcastTask::~EndBroadcastTask()
    {
        FUNCTION_ENTRY( "~EndBroadcastTask" );
        FUNCTION_EXIT;
    }


    void EndBroadcastTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.endBroadcastSingleTrain( m_trainId,
                                            m_announcementId,
                                            m_sessionId,
                                            m_origin );

        FUNCTION_EXIT;
    }


    EndLiveAnnouncementTask::EndLiveAnnouncementTask( ITimsPaManagerTaskCallback& callback,
                                                      TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                      TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                      const std::string& sessionId,
                                                      unsigned char origin )
            : m_callback( callback ),
              m_trainId( trainId ),
              m_announcementId( announcementId ),
              m_sessionId( sessionId ),
              m_origin( origin )

    {
        FUNCTION_ENTRY( "EndLiveAnnouncementTask" );
        FUNCTION_EXIT;
    }


    EndLiveAnnouncementTask::~EndLiveAnnouncementTask()
    {
        FUNCTION_ENTRY( "~EndLiveAnnouncementTask" );
        FUNCTION_EXIT;
    }


    void EndLiveAnnouncementTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.endLiveAnnouncementSingleTrain( m_trainId,
                                                   m_announcementId,
                                                   m_sessionId,
                                                   m_origin );

        FUNCTION_EXIT;
    }


    PreRecordedAnnouncementTask::PreRecordedAnnouncementTask( ITimsPaManagerTaskCallback& callback,
                                                              TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                              TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                              //unsigned char messageNumber,
                                                              unsigned short messageNumber,
                                                              //unsigned short libraryVersion,
                                                              unsigned long libraryVersion,
                                                              TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode,
                                                              const std::string& sessionId,
                                                              unsigned char origin,
															  TA_Base_Bus::ITrainPaCorba::EDvaMsgPriority messagePriority)
            : m_callback( callback ),
              m_trainId( trainId ),
              m_announcementId( announcementId ),
              m_messageNumber( messageNumber ),
              m_libraryVersion( libraryVersion ),
              m_requestMode( requestMode ),
              m_sessionId( sessionId ),
              m_origin( origin ),
			  m_messagePriority( messagePriority )
    {
        FUNCTION_ENTRY( "PreRecordedAnnouncementTask" );
        FUNCTION_EXIT;
    }


    PreRecordedAnnouncementTask::~PreRecordedAnnouncementTask()
    {
        FUNCTION_ENTRY( "~PreRecordedAnnouncementTask" );
        FUNCTION_EXIT;
    }


    void PreRecordedAnnouncementTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.preRecordedAnnouncementSingleTrain( m_trainId,
                                                       m_announcementId,
                                                       m_messageNumber,
                                                       m_libraryVersion,
                                                       m_requestMode,
                                                       m_sessionId,
                                                       m_origin,
													   m_messagePriority);

        FUNCTION_EXIT;
    }


    SetupLiveAnnouncementMonitorTask::SetupLiveAnnouncementMonitorTask( IRadioGroupCoordinator& radioGroupCoordinator,
                                                                        unsigned long numberOfTrains,
                                                                        const std::string& radioGroup )
            : m_radioGroupCoordinator( radioGroupCoordinator ),
              m_numberOfTrainsSuccessful( 0 ),
              m_radioGroup( radioGroup ),
              m_barrier( numberOfTrains )
    {
        FUNCTION_ENTRY( "SetupLiveAnnouncementMonitorTask" );
        FUNCTION_EXIT;
    }


    SetupLiveAnnouncementMonitorTask::~SetupLiveAnnouncementMonitorTask()
    {
        FUNCTION_ENTRY( "~SetupLiveAnnouncementMonitorTask" );
        FUNCTION_EXIT;
    }


    void SetupLiveAnnouncementMonitorTask::signalSetupLiveAnnouncementSuccess()
    {
        FUNCTION_ENTRY( "signalSetupLiveAnnouncementSuccess" );

        // increment the number of successful trains and post the barrier

        ++m_numberOfTrainsSuccessful;

        m_barrier.post();

        FUNCTION_EXIT;
    }


    void SetupLiveAnnouncementMonitorTask::signalSetupLiveAnnouncementFailure()
    {
        FUNCTION_ENTRY( "signalSetupLiveAnnouncementFailure" );

        // just post the barrier
        m_barrier.post();

        FUNCTION_EXIT;
    }


    void SetupLiveAnnouncementMonitorTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        // wait on m_barrier
        m_barrier.wait();

        // when it unblocks, check m_numberOfSuccessfulTrains

        if ( 0 == m_numberOfTrainsSuccessful )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "No trains in setup live announcement were successful. Unlocking group %s",
                         m_radioGroup.c_str() );

            m_radioGroupCoordinator.unlockRadioGroup( m_radioGroup );
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "%d trains in setup live announcement were successful. Group %s remains locked",
                         m_numberOfTrainsSuccessful, m_radioGroup.c_str() );
        }

        FUNCTION_EXIT;
    }


    SetupLiveAnnouncementTask::SetupLiveAnnouncementTask( ITimsPaManagerTaskCallback& callback,
                                                          TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                          TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                          TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode,
                                                          const std::string& staticGroup,
                                                          const std::string& sessionId,
                                                          unsigned char origin,
                                                          SetupLiveAnnouncementMonitorTaskPtr monitorTask )
            : m_callback( callback ),
              m_trainId( trainId ),
              m_announcementId( announcementId ),
              m_requestMode( requestMode ),
              m_staticGroup( staticGroup ),
              m_sessionId( sessionId ),
              m_origin( origin ),
              m_monitorTask( monitorTask )
    {
        FUNCTION_ENTRY( "SetupLiveAnnouncementTask" );
        FUNCTION_EXIT;
    }


    SetupLiveAnnouncementTask::~SetupLiveAnnouncementTask()
    {
        FUNCTION_ENTRY( "~SetupLiveAnnouncementTask" );
        FUNCTION_EXIT;
    }


    void SetupLiveAnnouncementTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.setupLiveAnnouncementSingleTrain( m_trainId,
                                                     m_announcementId,
                                                     m_requestMode,
                                                     m_staticGroup,
                                                     m_sessionId,
                                                     m_origin,
                                                     m_monitorTask );

        FUNCTION_EXIT;
    }
}
