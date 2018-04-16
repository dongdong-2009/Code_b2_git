/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/signs/tis_agent_access/src/TrainDataVersions.cpp $
  * @author:  Kent Yip
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/03/06 19:23:10 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Process and data encapsulation to store the following versions for a train:
  *     1. Predefined TTIS Library Version
  *     2. Next Predefined TTIS Library Version
  *     3. Schedule Version
  *     4. Next Schedule Version
  *
  * This is necessary due to the fact that the TrainAgent does not send an
  * TA_Base_Bus::TISNotification with ALL the fileds set correctly.
  *
  */
#include "TrainDataVersions.h"

namespace TA_IRS_Bus
{
    
    TrainDataVersions::TrainDataVersions()
        : m_trainDataVersions(),
          m_lock()
    {
    }
    

    TrainDataVersions::~TrainDataVersions()
    {
    }
    

    TA_Base_Core::TrainDataVersion TrainDataVersions::getTrainDataVersion( unsigned short trainID ) const
    {
        FUNCTION_ENTRY( "getTrainDataVersion" );

        TA_THREADGUARD( m_lock );

        for ( std::vector< TA_Base_Core::TrainDataVersion >::const_iterator it = m_trainDataVersions.begin();
              it != m_trainDataVersions.end(); ++it )
        {
            if ( it->trainNumber == trainID )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "[train id=%d][current version=%d][next version=%d][current shcedule=%d][next schedule=%d]",
                    trainID, it->predefinedLibraryVersion, it->nextPredefinedLibraryVersion, it->trainTimeScheduleVersion, it->nextTrainTimeScheduleVersion  );

                FUNCTION_EXIT;
                return *it;
            }
        }

        // Train does not exist ...
        throw TrainDoesNotExistException();
    }
    

    std::vector< TA_Base_Core::TrainDataVersion > TrainDataVersions::getTrainDataVersions() const
    {
        FUNCTION_ENTRY( "getTrainDataVersions" );

        TA_THREADGUARD( m_lock );

        FUNCTION_EXIT;
        return this->m_trainDataVersions;
    }
    

    bool TrainDataVersions::updateTTISLibraryVersions( unsigned short trainID,
                                                       unsigned long libraryVersion,
                                                       unsigned long nextLibraryVersion )
    {
        FUNCTION_ENTRY( "updateTTISLibraryVersions" );

        TA_THREADGUARD( m_lock );

        for ( std::vector< TA_Base_Core::TrainDataVersion >::iterator it = m_trainDataVersions.begin();
              it != m_trainDataVersions.end(); ++it )
        {
            if ( it->trainNumber == trainID )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "updated library versions to [train id=%d][current version=%d][next version=%d]",
                    trainID, libraryVersion, nextLibraryVersion );

                it->predefinedLibraryVersion = libraryVersion;
                it->nextPredefinedLibraryVersion = nextLibraryVersion;

                FUNCTION_EXIT;
                return false;
            }
        }
        
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
          "Adding train: [train id=%d][current version=%d][next version=%d]",
          trainID, libraryVersion, nextLibraryVersion );

        TA_Base_Core::TrainDataVersion data;
        data.trainNumber = static_cast< unsigned char >( trainID );
        data.predefinedLibraryVersion = libraryVersion;
        data.nextPredefinedLibraryVersion = nextLibraryVersion;
        data.trainTimeScheduleVersion = 0;
        data.nextTrainTimeScheduleVersion = 0;
        m_trainDataVersions.push_back( data );

        FUNCTION_EXIT;
        return true;
    }
    

    bool TrainDataVersions::updateScheduleVersions( unsigned short trainID,
                                                    unsigned short scheduleVersion,
                                                    unsigned short nextScheduleVersion )
    {
        FUNCTION_ENTRY( "updateScheduleVersions" );

        TA_THREADGUARD( m_lock );

        for ( std::vector< TA_Base_Core::TrainDataVersion >::iterator it = this->m_trainDataVersions.begin();
              it != this->m_trainDataVersions.end(); ++it )
        {
            if ( it->trainNumber == trainID )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "updated schedule versions to [train id=%d][current schedule=%d][next schedule=%d]",
                    trainID, scheduleVersion, nextScheduleVersion );

                it->trainTimeScheduleVersion = scheduleVersion;
                it->nextTrainTimeScheduleVersion = nextScheduleVersion;

                FUNCTION_EXIT;
                return false;
            }
        }
        
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Adding train: [train id=%d][current schedule=%d][next schedule=%d]",
            trainID, scheduleVersion, nextScheduleVersion );

        TA_Base_Core::TrainDataVersion data;
        data.trainNumber = static_cast< unsigned char >( trainID );
        data.trainTimeScheduleVersion = scheduleVersion;
        data.nextTrainTimeScheduleVersion = nextScheduleVersion;
        data.predefinedLibraryVersion = 0;
        data.nextPredefinedLibraryVersion = 0;
        m_trainDataVersions.push_back( data );

        FUNCTION_EXIT;
        return true;
    }
    
} // namespace TA_IRS_Bus
