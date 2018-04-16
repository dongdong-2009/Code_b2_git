/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source$
 * @author:  Adam Radics
 * @version: $Revision$
 *
 * Last modification: $Date$
 * Last modified by:  $Author$
 *
 */

#include "app/trains/train_agent/src/TrainCommsState.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip>

namespace TA_IRS_App
{
    const std::string TrainCommsState::EMPTY_SERVICE_NUMBER_SUFFIX = "/_";

    TrainCommsState::TrainCommsState( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                      const std::string& primaryTsi,
                                      const std::string& secondaryTsi,
                                      unsigned short paLibraryVersion,
                                      unsigned short ttisLibraryVersion,
                                      unsigned short ttisScheduleVersion )
            : TA_IRS_Bus::CoreTrainState( trainId, primaryTsi ),
              m_activeMpu( TA_IRS_Bus::ProtocolCommonTypes::MpuMain ),
              m_oa1Valid( false ),
              m_atcValid( false ),
              m_communicatingLocation( 0 ),
              m_paLibraryVersion( paLibraryVersion ),
              m_ttisLibraryVersion( ttisLibraryVersion ),
              m_ttisScheduleVersion( ttisScheduleVersion ),
              m_atsValid( false ),
              m_atsLocalisationValid( false ),
              m_atsTrack( TA_IRS_Bus::TrainInformationTypes::AtsTrackUnknown ),
              m_currentCctvZone(),
              m_serviceNumber( "" ),
              m_isMute( false ),
              m_persistedStateChanged( false )
    {
        FUNCTION_ENTRY( "TrainCommsState" );

        m_mpuTsi[0] = primaryTsi;
        m_mpuTsi[1] = secondaryTsi;
        
        createEmptyServiceNumber();

        FUNCTION_EXIT;
    }


    TrainCommsState::~TrainCommsState()
    {
        FUNCTION_ENTRY( "~TrainCommsState" );
        FUNCTION_EXIT;
    }


    const std::string& TrainCommsState::getActiveTsi() const
    {
        FUNCTION_ENTRY( "getActiveTsi" );
        FUNCTION_EXIT;
        return m_mpuTsi[ m_activeMpu ];
    }


    const std::string& TrainCommsState::getStandbyTsi() const
    {
        FUNCTION_ENTRY( "getStandbyTsi" );

        if ( TA_IRS_Bus::ProtocolCommonTypes::MpuMain == m_activeMpu )
        {
            FUNCTION_EXIT;
            return m_mpuTsi[ TA_IRS_Bus::ProtocolCommonTypes::MpuBackup ];
        }

        FUNCTION_EXIT;
        return m_mpuTsi[ TA_IRS_Bus::ProtocolCommonTypes::MpuMain ];
    }


    const std::string& TrainCommsState::getPrimaryTsi() const
    {
        FUNCTION_ENTRY( "getPrimaryTsi" );
        FUNCTION_EXIT;
        return m_mpuTsi[ TA_IRS_Bus::ProtocolCommonTypes::MpuMain ];
    }


    const std::string& TrainCommsState::getSecondaryTsi() const
    {
        FUNCTION_ENTRY( "getSecondaryTsi" );
        FUNCTION_EXIT;
        return m_mpuTsi[ TA_IRS_Bus::ProtocolCommonTypes::MpuBackup ];
    }


    TA_IRS_Bus::ProtocolCommonTypes::ETimsOrigin TrainCommsState::getActiveMpu() const
    {
        FUNCTION_ENTRY( "getActiveMpu" );
        FUNCTION_EXIT;
        return m_activeMpu;
    }


    unsigned short TrainCommsState::getPaLibraryVersion() const
    {
        FUNCTION_ENTRY( "getPaLibraryVersion" );
        FUNCTION_EXIT;
        return m_paLibraryVersion;
    }


    unsigned short TrainCommsState::getTtisLibraryVersion() const
    {
        FUNCTION_ENTRY( "getTtisLibraryVersion" );
        FUNCTION_EXIT;
        return m_ttisLibraryVersion;
    }


    unsigned short TrainCommsState::getTtisScheduleVersion() const
    {
        FUNCTION_ENTRY( "getTtisScheduleVersion" );
        FUNCTION_EXIT;
        return m_ttisScheduleVersion;
    }


    bool TrainCommsState::isAtcValid() const
    {
        FUNCTION_ENTRY( "isAtcValid" );
        FUNCTION_EXIT;
        return m_atcValid;
    }


    bool TrainCommsState::isOa1Valid() const
    {
        FUNCTION_ENTRY( "isOa1Valid" );
        FUNCTION_EXIT;
        return m_oa1Valid;
    }


    bool TrainCommsState::isTrainCommunicationValid() const
    {
        FUNCTION_ENTRY( "isTrainCommunicationValid" );
        FUNCTION_EXIT;
        return isAtcValid() || isOa1Valid();
    }


    unsigned long TrainCommsState::getCommunicatingLocation() const
    {
        FUNCTION_ENTRY( "getCommunicatingLocation" );
        FUNCTION_EXIT;
        return m_communicatingLocation;
   }


   bool TrainCommsState::isAtsValid() const
    {
        FUNCTION_ENTRY( "isAtsValid" );
        FUNCTION_EXIT;
        return m_atsValid;
    }


    bool TrainCommsState::isLocalisationValid() const
    {
        FUNCTION_ENTRY( "isLocalisationValid" );
        FUNCTION_EXIT;
        return m_atsLocalisationValid;
    }


    bool TrainCommsState::isMute() const
    {
        FUNCTION_ENTRY( "isMute" );
        FUNCTION_EXIT;
        return m_isMute;
    }


    CctvZone TrainCommsState::getCurrentCctvZone() const
    {
        FUNCTION_ENTRY( "getCurrentCctvZone" );
        FUNCTION_EXIT;
        return m_currentCctvZone;
    }


    TA_IRS_Bus::TrainInformationTypes::EAtsTrack TrainCommsState::getAtsTrack() const
    {
        FUNCTION_ENTRY( "getAtsTrack" );
        FUNCTION_EXIT;
        return m_atsTrack;
    }


    std::string TrainCommsState::getServiceNumber() const
    {
        FUNCTION_ENTRY( "getServiceNumber" );
        FUNCTION_EXIT;
        return m_serviceNumber;
    }


    bool TrainCommsState::isThisTrain( const std::string& itsi ) const
    {
        FUNCTION_ENTRY( "isThisTrain" );

        // if this tsi is empty, return false
        // if this tsi matches anything in m_mpuTsi, return true
        FUNCTION_EXIT;
        return ( ( false == itsi.empty() ) &&
                 ( ( getPrimaryTsi() == itsi ) ||
                   ( getSecondaryTsi() == itsi ) ) );
    }


    TA_IRS_Bus::TrainInformationTypes::TrainDetails TrainCommsState::getTrainDetails()
    {
        FUNCTION_ENTRY( "getTrainDetails" );

        TA_IRS_Bus::TrainInformationTypes::TrainDetails details;
        details.trainId = getTrainId();
        details.communicationValid = isTrainCommunicationValid();
        details.communicatingLocation = m_communicatingLocation;
        details.primaryTsi = getActiveTsi();        // the TSI of the MPU in control
        details.secondaryTsi = getStandbyTsi();     // the TSI of the standby MPU
        details.atsValid = m_atsValid;
        details.isLocalisationValid = m_atsLocalisationValid;
        details.currentLocation = getLocation();
        details.currentCctvZone = m_currentCctvZone.getCurrentStage();
        details.serviceNumber = m_serviceNumber;
        details.isMute = m_isMute;
        details.track = m_atsTrack;

        FUNCTION_EXIT;
        return details;
    }


    bool TrainCommsState::hasPeristedStateChanged() const
    {
        FUNCTION_ENTRY( "hasPeristedStateChanged" );
        FUNCTION_EXIT;
        return m_persistedStateChanged;
    }


    void TrainCommsState::setTrainTsi( TA_IRS_Bus::ProtocolCommonTypes::ETimsOrigin mpu,
                                       const std::string& tsi,
                                       time_t timestamp,
                                       bool setAsPrimary )
    {
        FUNCTION_ENTRY( "setTrainTsi" );

        // protect all places m_activeMpu can be set to ensure it is always valid
        TA_ASSERT( ( TA_IRS_Bus::ProtocolCommonTypes::MpuMain == mpu ) ||
                   ( TA_IRS_Bus::ProtocolCommonTypes::MpuBackup == mpu ),
                   "Invalid TIMS Origin" );

        // If m_mpuTsi[mpu] is not the same as the given TSI
        if ( tsi != m_mpuTsi[ mpu ] )
        {
            // set m_mpuTsi[mpu] to the TSI
            m_mpuTsi[ mpu ] = tsi;

            m_persistedStateChanged = true;

            setDataChangedWithTimestamp( timestamp );
        }

        // if this is to become the active MPU
        if ( true == setAsPrimary )
        {
            // set the master radio id
            setRadioId( tsi );

            // set the active MPU if its not already matching
            // if this is to become the active MPU
            if ( mpu != m_activeMpu )
            {
                m_activeMpu = mpu;

                setDataChangedWithTimestamp( timestamp );
            }
        }

        FUNCTION_EXIT;
    }


    void TrainCommsState::setPaLibraryVersion( unsigned short version,
                                               time_t timestamp )
    {
        FUNCTION_ENTRY( "setPaLibraryVersion" );

        if ( version != m_paLibraryVersion )
        {
            m_paLibraryVersion = version;
            m_persistedStateChanged = true;
            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainCommsState::setTtisLibraryVersion( unsigned short version,
                                                 time_t timestamp )
    {
        FUNCTION_ENTRY( "setTtisLibraryVersion" );

        if ( version != m_ttisLibraryVersion )
        {
            m_ttisLibraryVersion = version;
            m_persistedStateChanged = true;
            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainCommsState::setTtisScheduleVersion( unsigned short version,
                                                  time_t timestamp )
    {
        FUNCTION_ENTRY( "setTtisScheduleVersion" );

        if ( version != m_ttisScheduleVersion )
        {
            m_ttisScheduleVersion = version;
            m_persistedStateChanged = true;
            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainCommsState::setLibraryVersions( unsigned short paLibraryVersion,
                                              unsigned short ttisLibraryVersion,
                                              unsigned short ttisTimeScheduleVersion,
                                              time_t timestamp )
    {
        FUNCTION_ENTRY( "setLibraryVersions" );

        // just calls each appropriate set method
        setPaLibraryVersion( paLibraryVersion, timestamp );
        setTtisLibraryVersion( ttisLibraryVersion, timestamp );
        setTtisScheduleVersion( ttisTimeScheduleVersion, timestamp );

        FUNCTION_EXIT;
    }


    void TrainCommsState::setAtcValidity( bool validity,
                                          time_t timestamp,
                                          unsigned long location )
    {
        FUNCTION_ENTRY( "setAtcValidity" );

        // first set the validity
        if ( validity != m_atcValid )
        {
            m_atcValid = validity;

            updateTrainValidity();

            setDataChangedWithTimestamp( timestamp );
        }

        // second set the location if the communication is valid
        if ( ( validity == true ) &&
             ( 0 != location ) &&
             ( m_communicatingLocation != location ) )
        {
            m_communicatingLocation = location;
            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainCommsState::setOa1Validity( bool validity,
                                          time_t timestamp,
                                          unsigned long location )
    {
        FUNCTION_ENTRY( "setOa1Validity" );

        // first set the validity
        if ( validity != m_oa1Valid )
        {
            m_oa1Valid = validity;

            updateTrainValidity();

            setDataChangedWithTimestamp( timestamp );
        }

        // second set the location if the communication is valid
        if ( ( validity == true ) &&
             ( 0 != location ) &&
             ( m_communicatingLocation != location ) )
        {
            m_communicatingLocation = location;
            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainCommsState::setAtsDetails( unsigned long currentLocation,
                                         double currentCctvZone,
                                         const std::string& serviceNumber,
                                         TA_IRS_Bus::TrainInformationTypes::EAtsTrack track,
                                         bool mute,
                                         bool localisationValid )
    {
        FUNCTION_ENTRY( "setAtsDetails" );

        CctvZone newCctvZone( currentCctvZone );

        // checks the given details against the currently stored details
        // and if different sets them

		//Tag: ServiceNumber
		//CL-20826 keep the service number new
		m_serviceNumber = serviceNumber;

        if ( ( false == m_atsValid ) ||
             ( getLocation() != currentLocation ) ||
             ( newCctvZone != m_currentCctvZone ) ||
/*             ( serviceNumber != m_serviceNumber ) ||*/ //wuzhongyi CL-20826
             ( track != m_atsTrack ) ||
             ( mute != m_isMute ) ||
             ( localisationValid != m_atsLocalisationValid ) )
        {
            m_atsValid = true;

            setLocation( currentLocation );

            m_atsLocalisationValid = localisationValid;
            m_atsTrack = track;
            m_currentCctvZone = newCctvZone;
            //m_serviceNumber = serviceNumber; wuzhongyi CL-20826, move up Tag: ServiceNumber
            m_isMute = mute;

            // update the validity
            updateTrainValidity();

            // any changes after this that affect validity or location need to trigger a state change
            // but this information is received by all agents so no state sync is required
            setSynchronisationComplete();
        }

        FUNCTION_EXIT;
    }


    void TrainCommsState::invalidateAts()
    {
        FUNCTION_ENTRY( "invalidateAts" );

        if ( true == m_atsValid )
        {
            m_atsValid = false;

            updateTrainValidity();
            
            // clear the old service number, no longer valid
            createEmptyServiceNumber();

            // any changes after this that affect validity or location need to trigger a state change
            // but this information is received by all agents so no state sync is required
            setSynchronisationComplete();
        }

        FUNCTION_EXIT;
    }


    void TrainCommsState::overrideCurrentLocation( unsigned long location,
                                                   unsigned long cctvZone )
    {
        FUNCTION_ENTRY( "overrideCurrentLocation" );

        CctvZone newCctvZone( cctvZone );

        if ( ( getLocation() != location ) ||
             ( newCctvZone != m_currentCctvZone ) )
        {
            setLocation( location );

            m_currentCctvZone = newCctvZone;

            // any changes after this that affect validity or location need to trigger a state change
            // but this information is received by all agents so no state sync is required
            setSynchronisationComplete();
        }

        FUNCTION_EXIT;
    }


    void TrainCommsState::resetPersistedStateChanged()
    {
        FUNCTION_ENTRY( "resetPersistedStateChanged" );

        m_persistedStateChanged = false;

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::StateUpdateTypes::TrainCommsSyncState TrainCommsState::getSyncState() const
    {
        FUNCTION_ENTRY( "getSyncState" );

        TA_IRS_Bus::StateUpdateTypes::TrainCommsSyncState syncState;

        syncState.trainId = getTrainId();
        syncState.primaryMpuTsi = m_mpuTsi[ TA_IRS_Bus::ProtocolCommonTypes::MpuMain ];
        syncState.secondaryMpuTsi = m_mpuTsi[ TA_IRS_Bus::ProtocolCommonTypes::MpuBackup ];
        syncState.activeMpu = m_activeMpu;
        syncState.paLibraryVersion = m_paLibraryVersion;
        syncState.ttisLibraryVersion = m_ttisLibraryVersion;
        syncState.ttisScheduleVersion = m_ttisScheduleVersion;
        syncState.oa1Valid = m_oa1Valid;
        syncState.atcValid = m_atcValid;
        syncState.communicatingLocation = m_communicatingLocation;
        syncState.timestamp = getTimestamp();

        FUNCTION_EXIT;
        return syncState;
    }


    void TrainCommsState::setSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainCommsSyncState& syncState )
    {
        FUNCTION_ENTRY( "setSyncState" );

        // If the timestamp in the given state structure is less than the getTimestamp() of this state,
        // discard the sync state structure - log an error and return.

        if ( syncState.timestamp < getTimestamp() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "Received old sync state for train %d [sync time: %d][state time: %d]",
                         getTrainId(), syncState.timestamp, getTimestamp() );

            FUNCTION_EXIT;
            return;
        }

        // otherwise, if the timestamp in the new state is newer
        // Set the fields from the bulk state structure.

        m_mpuTsi[ TA_IRS_Bus::ProtocolCommonTypes::MpuMain ] = syncState.primaryMpuTsi;
        m_mpuTsi[ TA_IRS_Bus::ProtocolCommonTypes::MpuBackup ] = syncState.secondaryMpuTsi;
        // no need to verify m_activeMpu - the corba conversion already did this
        m_activeMpu = syncState.activeMpu;
        m_paLibraryVersion = syncState.paLibraryVersion;
        m_ttisLibraryVersion = syncState.ttisLibraryVersion;
        m_ttisScheduleVersion = syncState.ttisScheduleVersion;
        m_oa1Valid = syncState.oa1Valid;
        m_atcValid = syncState.atcValid;
        m_communicatingLocation = syncState.communicatingLocation;

        // in case the active MPU changed
        setRadioId( m_mpuTsi[ m_activeMpu ] );

        // the data has changed, but a sync is not required
        setDataChangedWithTimestamp( syncState.timestamp, false );

        FUNCTION_EXIT;
    }


    void TrainCommsState::updateTrainValidity()
    {
        FUNCTION_ENTRY( "updateTrainValidity" );

        // validity is determined by 3 criteria
        bool valid = m_oa1Valid || m_atcValid || m_atsValid;

        if ( isValid() != valid )
        {
            setValidity( valid );

            // the data changed, so notify observers
            // but state sync is not required
            setDataChanged( false );
        }

        FUNCTION_EXIT;
    }


    void TrainCommsState::createEmptyServiceNumber()
    {
        FUNCTION_ENTRY( "createEmptyServiceNumber" );
        
        std::ostringstream serviceNumber;
        serviceNumber << static_cast< unsigned int >( getTrainId() )
                      << EMPTY_SERVICE_NUMBER_SUFFIX;
        m_serviceNumber = serviceNumber.str();
        
        FUNCTION_EXIT;
    }

} // TA_IRS_App
