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
#if !defined(TrainCommsState_H)
#define TrainCommsState_H

#include "app/trains/train_agent/src/CctvZone.h"

#include "bus/trains/TrainTransactionModel/src/CoreTrainState.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"

#include <string>
#include <ctime>

namespace TA_IRS_App
{

    /**
     * This holds the ATS/radio information for the train. The most basic and essential data.
     *
     * This is a global state and is synchronised across all train agents
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:26 PM
     */
    class TrainCommsState : public TA_IRS_Bus::CoreTrainState
    {

    public:

        /**
         * Creates the object, optionally setting the persisted values for the train's TSI.
         *
         * @param trainId
         * @param primaryTsi    Ths TSI of the primary MPU
         * @param secondaryTsi    The TSI of the second MPU
         * @param paLibraryVersion    The persisted PA library version
         * @param ttisLibraryVersion    The TTIS library version persisted in the database
         * @param ttisScheduleVersion    The TTIS time schedule version persisted in the database
         */
        TrainCommsState( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                         const std::string& primaryTsi = "",
                         const std::string& secondaryTsi = "",
                         unsigned short paLibraryVersion = 0,
                         unsigned short ttisLibraryVersion = 0,
                         unsigned short ttisScheduleVersion = 0 ,
						 unsigned short nextTtisLibraryVersion = 0,
						 unsigned short nextTtisScheduleVersion = 0 );

		
		/**
		 * Copy Constructor
		 */

		TrainCommsState( const TrainCommsState& trainCommState );


        /**
         * Destructor
         */
        virtual ~TrainCommsState();


        /**
         * Gets the TSI of the active MPU
         *
         * @return the Primary TSI string
         */
        const std::string& getActiveTsi() const;


        /**
         * Gets the TSI of the standby MPU (the MPU not in control)
         * 
         * @return the Secondary TSI string
         */
        const std::string& getStandbyTsi() const;


        /**
         * Gets the TSI of the primary MPU
         * Note this is not the active TSI, this is the primary MPU tsi (which may not be the current control mode MPU).
         * Use getActiveTsi to get the active TSI.
         *
         * @return the Primary TSI string
         */
        const std::string& getPrimaryTsi() const;


        /**
         * Gets the TSI of the secondary MPU
         * Note this is not the standby TSI, this is the secondary MPU tsi (which may in fact be the MPU in control).
         * Use getStandbyTsi to get the standby TSI.
         * @return the Secondary TSI string
         */
        const std::string& getSecondaryTsi() const;


        /**
         * Gets whether the MPU is the main or backup MPU
         *
         * @return the active MPU
         */
        TA_IRS_Bus::ProtocolCommonTypes::ETimsOrigin getActiveMpu() const;


        /**
         * Gets the PA message library version
         *
         * @return the PA message library version on the train
         */
        unsigned short getPaLibraryVersion() const;


        /**
         * Gets the TTIS library version on this train
         *
         * @return the TTIS library version on this train
         */
        unsigned short getTtisLibraryVersion() const;

		 /**
         * Gets the next TTIS library version on this train
         *
         * @return the TTIS library version on this train
         */
        unsigned short getNextTtisLibraryVersion() const;


        /**
         * Gets the TTIS time schedule version on this train
         *
         * the TTIS time schedule version on this train
         */
        unsigned short getTtisScheduleVersion() const;

		 /**
         * Gets the next TTIS time schedule version on this train
         *
         * the TTIS time schedule version on this train
         */
        unsigned short getNextTtisScheduleVersion() const;


        /**
         * Gets whether the train is sending ATC tables.
         *
         * @return whether the train is sending ATC
         */
        bool isAtcValid() const;


        /**
         * Gets whether the train is sending OA1 tables.
         *
         * @return whether the train is sending OA1
         */
        bool isOa1Valid() const;


        /**
         * Returns true if the train is actively communicating with an agent
         *
         * @return whether the train is valid via the radio path
         */
        bool isTrainCommunicationValid() const;


        /**
         * Gets the location the train is sending/last sent messages to
         *
         * @return the last communicated location
         */
        unsigned long getCommunicatingLocation() const;


        /**
         * Gets whether the train is in ATS
         *
         * @return whether the train is present in ATS
         */
        bool isAtsValid() const;


        /**
         * Gets whether the train is localised in ATS
         *
         * @return whether the train is present and localised in ATS
         */
        bool isLocalisationValid() const;


        /**
         * Gets whether the train is mute in ATS
         *
         * @return whether the train is mute in ATS
         */
        bool isMute() const;


        /**
         * Gets the cctv zone the train is in.
         *
         * @return the current CCTV zone reported by ATS
         */
        CctvZone getCurrentCctvZone() const;


        /**
         * Gets the track the train is on.
         *
         * @return the ATS track the train is on from ATS
         */
        TA_IRS_Bus::TrainInformationTypes::EAtsTrack getAtsTrack() const;


        /**
         * Gets the train's service number
         *
         * @return the trains service number reported by ATS (in the format PV/Service)
         */
        std::string getServiceNumber() const;


        /**
         * Checks the given ITSI to see if it is owned by this train
         *
         * @return true if the ITSI belongs to this train
         *
         * @param itsi    the itsi to check
         */
        bool isThisTrain( const std::string& itsi ) const;


        /**
         * Gets the train details that will be sent for client updates
         *
         * @return the information required for train updates
         */
        TA_IRS_Bus::TrainInformationTypes::TrainDetails getTrainDetails();


        /**
         * Checks whether the persisted state portion of this object needs to be written to the db
         *
         * @return true if state needs to be persisted to the database
         */
        bool hasPeristedStateChanged() const;


        /**
         * Sets the TSI associated with the given MPU. Optionally sets this as the active MPU.
         *
         * @param mpu    The MPU to set
         * @param tsi    the TSI for this MPU
         * @param timestamp    The timestamp on the incoming TIMS message
         * @param setAsPrimary    If true, this MPU will be the primary
         */
        void setTrainTsi( TA_IRS_Bus::ProtocolCommonTypes::ETimsOrigin mpu,
                          const std::string& tsi,
                          time_t timestamp,
                          bool setAsPrimary = true );


        /**
         * Sets the PA message library version
         *
         * @param version    The new version number
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        void setPaLibraryVersion( unsigned short version,
                                  time_t timestamp );


        /**
         * Sets the TTIS library version on this train
         *
         * @param version    The new library version
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        void setTtisLibraryVersion( unsigned short version,
                                    time_t timestamp );

		/**
         * Sets the next TTIS library version on this train
         *
         * @param version    The new library version
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        void setNextTtisLibraryVersion( unsigned short version,
                                    time_t timestamp );



        /**
         * Sets the TTIS schedule version on this train
         *
         * @param version    The new schedule version
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        void setTtisScheduleVersion( unsigned short version,
                                     time_t timestamp );

		 /**
         * Sets the next TTIS schedule version on this train
         *
         * @param version    The new schedule version
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        void setNextTtisScheduleVersion( unsigned short version,
                                     time_t timestamp );



        /**
         * Sets all train library versions in one go
         *
         * @param paLibraryVersion    The new PA library version number
         * @param ttisLibraryVersion    The new TTIS library version number
         * @param ttisTimeScheduleVersion    The new TTIS time schedule version number
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        void setLibraryVersions( unsigned short paLibraryVersion,
                                 unsigned short ttisLibraryVersion,
                                 unsigned short ttisTimeScheduleVersion,								 
                                 time_t timestamp );


        /**
         * Sets the atc validity
         *
         * @param validity
         * @param timestamp    The timestamp on the incoming TIMS message
         * @param location    The communicating location (this agent location) if the communication is valid.
         */
        void setAtcValidity( bool validity,
                             time_t timestamp,
                             unsigned long location = 0 );


        /**
         * Sets the oa1 validity
         *
         * @param validity
         * @param timestamp    The timestamp on the incoming TIMS message
         * @param location    The communicating location (this agent location) if the communication is valid.
         */
        void setOa1Validity( bool validity,
                             time_t timestamp,
                             unsigned long location = 0 );


        /**
         * Sets the ATS data, and sets the ATS data valid.
         *
         * @param currentLocation    The train's location
         * @param currentCctvZone    The train's cctv zone
         * @param serviceNumber    The train's service number
         * @param track    The track the train is on
         * @param mute    Whether the train is mute in ats or not
         * @param localisationValid    Whether the train is localised
         */
        void setAtsDetails( unsigned long currentLocation,
                            double currentCctvZone,
                            const std::string& serviceNumber,
                            TA_IRS_Bus::TrainInformationTypes::EAtsTrack track,
                            bool mute,
                            bool localisationValid );


        /**
         * Sets the ats validity to false
         */
        void invalidateAts();


        /**
         * Overrides the current location to one the operator specified
         *
         * @param location    The transactive location
         * @param cctvZone    The CCTV zone/stage ID
         */
        void overrideCurrentLocation( unsigned long location,
                                      unsigned long cctvZone );
        
        
        /**
         * Resets the flag to indicate any change in persisted state has
         * been persisted to the database.
         */
        void resetPersistedStateChanged();


        /**
         * Gets the state for synchronisation purposes
         *
         * @return the state needing to be synchronised
         */
        TA_IRS_Bus::StateUpdateTypes::TrainCommsSyncState getSyncState() const;


        /**
         * Sets the sync state for synchronisation purposes
         *
         * @param syncState    The new internal state of this object
         */
        void setSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainCommsSyncState& syncState );


        /**
         * This represents no service number for a train.
         */
        static const std::string EMPTY_SERVICE_NUMBER_SUFFIX;


    private:

        /**
         * Updates whether this train is considered a valid train or not
         */
        void updateTrainValidity();


        /**
         * generates the empty service number in the absence of one from ats
         */
        void createEmptyServiceNumber();


        /**
         * Index into m_mpuTsi of the active MPU.
         */
        TA_IRS_Bus::ProtocolCommonTypes::ETimsOrigin m_activeMpu;


        /**
         * The TSI's of the train.
         */
        std::string m_mpuTsi[2];


        /**
         * true if the train is sending OA1 tables.
         */
        bool m_oa1Valid;


        /**
         * true if the train is sending ATC tables.
         */
        bool m_atcValid;


        /**
         * This is where the train is currently sending OA1/ATC tables.
         * If the communication is not valid, this is the last place they were sent,
         * or 0 if they were never sent.
         */
        unsigned long m_communicatingLocation;


        /**
         * The PA library version on the train.
         */
        unsigned short m_paLibraryVersion;


        /**
         * The version of the predefined message library on the train
         */
        unsigned short m_ttisLibraryVersion;

		/**
         * The version of the next predefined message library on the train
         */
		unsigned short m_nextTtisLibraryVersion ;


        /**
         * The version of the time schedule library on the train
         */
        unsigned short m_ttisScheduleVersion;

		/**
		* The version of the next time schedule library on the train
         */

		unsigned short m_nextTtisScheduleVersion ;


        /**
         * True if there is valid ATS data
         */
        bool m_atsValid;


        /**
         * True if the ats localisation is good
         */
        bool m_atsLocalisationValid;


        /**
         * The track ATS says the train is on
         */
        TA_IRS_Bus::TrainInformationTypes::EAtsTrack m_atsTrack;


        /**
         * The current CCTV zone.
         */
        CctvZone m_currentCctvZone;


        /**
         * The train's service number.
         */
        std::string m_serviceNumber;


        /**
         * Whether the train is mute.
         */
        bool m_isMute;
        
        
        /**
         * Flag to indicate whether the persisted state has changed
         * if true, the data needs to be written to the database.
         */
        bool m_persistedStateChanged;

    };


    /**
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:27 PM
     */
    typedef boost::shared_ptr< TrainCommsState > TrainCommsStatePtr;

} // TA_IRS_App

#endif // !defined(TrainCommsState_H)
