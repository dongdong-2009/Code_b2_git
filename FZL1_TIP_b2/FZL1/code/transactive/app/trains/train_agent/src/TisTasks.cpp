/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "app/trains/train_agent/src/TisTasks.h"
#include "app/trains/train_agent/src/ITimsTisManagerTaskCallback.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

    FreeTextMessageTask::FreeTextMessageTask( ITimsTisManagerTaskCallback& callback,
                                              TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                              unsigned char origin,
                                              const TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters& displayParameters,
                                              time_t timestamp,
                                              const std::string& sessionId )
          : m_callback( callback ),
            m_trainId( trainId ),
            m_origin( origin ),
            m_displayParameters( displayParameters ),
            m_timestamp( timestamp ),
            m_sessionId( sessionId )
    {
        FUNCTION_ENTRY( "FreeTextMessageTask" );
        FUNCTION_EXIT;
    }


    FreeTextMessageTask::~FreeTextMessageTask()
    {
        FUNCTION_ENTRY( "~FreeTextMessageTask" );
        FUNCTION_EXIT;
    }


    void FreeTextMessageTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.displayFreeTextMessage( m_trainId,
                                           m_origin,
                                           m_displayParameters,
                                           m_timestamp,
                                           m_sessionId );

        FUNCTION_EXIT;
    }


    StoredMessageTask::StoredMessageTask( ITimsTisManagerTaskCallback& callback,
                                          TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                          unsigned char origin,
                                          const TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters& displayParameters,
                                          time_t timestamp,
                                          const std::string& sessionId )
          : m_callback( callback ),
            m_trainId( trainId ),
            m_origin( origin ),
            m_displayParameters( displayParameters ),
            m_timestamp( timestamp ),
            m_sessionId( sessionId )
    {
        FUNCTION_ENTRY( "StoredMessageTask" );
        FUNCTION_EXIT;
    }


    StoredMessageTask::~StoredMessageTask()
    {
        FUNCTION_ENTRY( "~StoredMessageTask" );
        FUNCTION_EXIT;
    }


    void StoredMessageTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.displayStoredMessage( m_trainId,
                                         m_origin,
                                         m_displayParameters,
                                         m_timestamp,
                                         m_sessionId );

        FUNCTION_EXIT;
    }


    ResetMessageTask::ResetMessageTask( ITimsTisManagerTaskCallback& callback,
                                        TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                        unsigned char origin,
                                        const TA_IRS_Bus::TtisTypes::TtisResetParameters& displayParameters,
                                        time_t timestamp,
                                        const std::string& sessionId )
          : m_callback( callback ),
            m_trainId( trainId ),
            m_origin( origin ),
            m_displayParameters( displayParameters ),
            m_timestamp( timestamp ),
            m_sessionId( sessionId )
    {
        FUNCTION_ENTRY( "ResetMessageTask" );
        FUNCTION_EXIT;
    }


    ResetMessageTask::~ResetMessageTask()
    {
        FUNCTION_ENTRY( "~ResetMessageTask" );
        FUNCTION_EXIT;
    }


    void ResetMessageTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.resetMessage( m_trainId,
                                 m_origin,
                                 m_displayParameters,
                                 m_timestamp,
                                 m_sessionId );

        FUNCTION_EXIT;
    }


    LibraryDownloadTask::LibraryDownloadTask( ITimsTisManagerTaskCallback& callback,
                                              TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                              unsigned char origin,
                                              const TA_IRS_Bus::TtisTypes::LibraryDataType& libraryData,
                                              unsigned short libraryVersion,
                                              const std::string& sessionId )
          : m_callback( callback ),
            m_trainId( trainId ),
            m_origin( origin ),
            m_libraryData( libraryData ),
            m_libraryVersion( libraryVersion ),
            m_sessionId( sessionId )
    {
        FUNCTION_ENTRY( "LibraryDownloadTask" );
        FUNCTION_EXIT;
    }


    LibraryDownloadTask::~LibraryDownloadTask()
    {
        FUNCTION_ENTRY( "~LibraryDownloadTask" );
        FUNCTION_EXIT;
    }


    void LibraryDownloadTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.libraryDownload( m_trainId,
                                    m_origin,
                                    m_libraryData,
                                    m_libraryVersion,
                                    m_sessionId );

        FUNCTION_EXIT;
    }


    LibraryUpgradeTask::LibraryUpgradeTask( ITimsTisManagerTaskCallback& callback,
                                            TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                            unsigned char origin,
                                            const std::string& sessionId )
          : m_callback( callback ),
            m_trainId( trainId ),
            m_origin( origin ),
            m_sessionId( sessionId )
    {
        FUNCTION_ENTRY( "LibraryUpgradeTask" );
        FUNCTION_EXIT;
    }


    LibraryUpgradeTask::~LibraryUpgradeTask()
    {
        FUNCTION_ENTRY( "~LibraryUpgradeTask" );
        FUNCTION_EXIT;
    }


    void LibraryUpgradeTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.libraryUpgrade( m_trainId,
                                   m_origin,
                                   m_sessionId );

        FUNCTION_EXIT;
    }


    ScheduleDownloadTask::ScheduleDownloadTask( ITimsTisManagerTaskCallback& callback,
                                                TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                unsigned char origin,
                                                const TA_IRS_Bus::TtisTypes::LibraryDataType& scheduleData,
                                                unsigned short scheduleVersion,
                                                const std::string& sessionId )
          : m_callback( callback ),
            m_trainId( trainId ),
            m_origin( origin ),
            m_scheduleData( scheduleData ),
            m_scheduleVersion( scheduleVersion ),
            m_sessionId( sessionId )
    {
        FUNCTION_ENTRY( "ScheduleDownloadTask" );
        FUNCTION_EXIT;
    }


    ScheduleDownloadTask::~ScheduleDownloadTask()
    {
        FUNCTION_ENTRY( "~ScheduleDownloadTask" );
        FUNCTION_EXIT;
    }


    void ScheduleDownloadTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.scheduleDownload( m_trainId,
                                     m_origin,
                                     m_scheduleData,
                                     m_scheduleVersion,
                                     m_sessionId );

        FUNCTION_EXIT;
    }


    ScheduleUpgradeTask::ScheduleUpgradeTask( ITimsTisManagerTaskCallback& callback,
                                              TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                              unsigned char origin,
                                              const std::string& sessionId )
          : m_callback( callback ),
            m_trainId( trainId ),
            m_origin( origin ),
            m_sessionId( sessionId )
    {
        FUNCTION_ENTRY( "ScheduleUpgradeTask" );
        FUNCTION_EXIT;
    }


    ScheduleUpgradeTask::~ScheduleUpgradeTask()
    {
        FUNCTION_ENTRY( "~ScheduleUpgradeTask" );
        FUNCTION_EXIT;
    }


    void ScheduleUpgradeTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.scheduleUpgrade( m_trainId,
                                    m_origin,
                                    m_sessionId );

        FUNCTION_EXIT;
    }

}
