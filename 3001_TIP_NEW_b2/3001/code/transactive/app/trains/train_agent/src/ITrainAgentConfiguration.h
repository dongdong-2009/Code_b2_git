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
#if !defined(ITrainAgentConfiguration_H)
#define ITrainAgentConfiguration_H

#include "app/trains/train_agent/src/ConfigurationTypes.h"
#include "app/trains/train_agent/src/CctvZone.h"

#include "bus/trains/TrainCommonLibrary/src/RadioGroupTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"

#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/TrainStateDai/src/IPersistedTrainState.h"

#include <string>

namespace TA_IRS_App
{

    /**
     * This holds the agent configuration derived from the local and the global entity data.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:52 PM
     */
    class ITrainAgentConfiguration
    {

    public:

        /**
         * Gets the entity key of this train agent entity
         *
         * @return the entity key of this entity
         */
        virtual unsigned long getEntityKey() = 0;


        /**
         * Gets the entity name of this train agent entity
         *
         * @return the entity name of this entity
         */
        virtual const std::string getEntityName() = 0;


        /**
         * Gets the entity type key of this train agent entity
         *
         * @return the entity type key of this entity
         */
        virtual unsigned long getEntityTypeKey() = 0;


        /**
         * Gets the locaiton key of this train agent entity
         *
         * @return the location key of this entity
         */
        virtual unsigned long getLocationKey() = 0;


        /**
         * Gets the type of location this agent is running at.
         *
         * @return whether this is an OCC, depot, or station.
         */
        virtual TA_Base_Core::ILocation::ELocationType getAgentLocationType() = 0;


        /**
         * This will return the subsystem key used to check access rights
         *
         * Depending on the agent location/type, different entities/keys are used for this.
         *
         * @return the subsystem key to use to process duty
         */
        virtual unsigned long getRightsSubsystem() = 0;


        /**
         * This will return the entity key used to check duty state.
         *
         * Depending on the agent location/type, different entities/keys are used for this.
         *
         * @return the entity key to use to process duty
         */
        virtual unsigned long getDutyEntity() = 0;


        /**
         * This will return the subsystem key used to check duty state.
         *
         * Depending on the agent location/type, different entities/keys are used for this.
         *
         * @return the subsystem key to use to process duty
         */
        virtual unsigned long getDutySubsystem() = 0;


        /**
         * This will return the subsystem key used to raise alarms.
         *
         * Depending on the agent location/type, different entities/keys are used for this.
         *
         * @return the subsystem key to use to process duty
         */
        virtual unsigned long getAlarmSubsystem() = 0;


        /**
         * Gets the agent asset name for raising audit messages
         *
         * @return the asset name
         */
        virtual std::string getAssetName() = 0;


        /**
         * This is the session ID the agent will use if it needs to do any access controlled action.
         * It is the super session ID and will always allow every action.
         *
         * @return the super session id
         */
        virtual const std::string& getAgentSessionId() = 0;


        /**
         * Gets the agent origin to send in TIMS messages from this agent.
         *
         * @return The origin value
         */
        virtual unsigned char getAgentOrigin() = 0;


        /**
         * This will load all the persisted train states from the database, and return them.
         *
         * @return the map of persisted train states.
         */
        virtual TA_IRS_Core::IPersistedTrainStateMap getPersistedTrainStates() = 0;


        /**
         * This will load all the train radio groups from the database, and return them.
         *
         * @return the map of radio groups.
         */
        virtual TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap getRadioGroups() = 0;


        /**
         * Gets the type of location for the given location key
         *
         * @return whether it is OCC, depot, or station.
         *
         * @param locationKey    The location to check
         */
        virtual TA_Base_Core::ILocation::ELocationType getLocationType( unsigned long locationKey ) = 0;


        /**
         * Given an operator session ID, get the console the operator is using.
         *
         * @return the console ID, or 0 if unknown.
         *
         * @param sessionId    The operator session ID
         */
        virtual unsigned long getConsoleFromSession( const std::string& sessionId ) = 0;


        /**
         * Given a console key, get the location for it.
         *
         * @return the location key for the given console.
         *
         * @param consoleKey    The console key
         */
        virtual unsigned long getLocationFromConsole( unsigned long consoleKey ) = 0;


        /**
         * Gets the console origin to send in TIMS messages originating from a particular operator action.
         *
         * @return The origin value, if not found the agent origin will be used
         *
         * @param consoleId    The console key
         */
        virtual unsigned char getConsoleOrigin( unsigned long consoleId ) = 0;


        /**
         * Gets the console origin to send in TIMS messages originating from a particular operator action.
         *
         * @return The origin value, if not found the agent origin will be used
         *
         * @param sessionId    The operator session ID
         */
        virtual unsigned char getOperatorOrigin( const std::string& sessionId ) = 0;


        /**
         * This will get all the locations of the given type
         *
         * @return a map of key to information
         *
         * @param locationType    The location type to get
         */
        virtual LocationInfoMap getAllLocationsOfType( TA_Base_Core::ILocation::ELocationType locationType ) = 0;


        /**
         * Checks whether the given location is next to the depot or not
         *
         * @return true if the location is adjactent to the depot
         */
        virtual bool isNextToDepot( unsigned long locationKey ) = 0;


        /**
         * Given a transactive location, get which CCTV zone it is in.
         *
         * @return the CCTV zone for the given location. If there is none, zone 0 will be returned.
         *
         * @param locationKey    The transactive location to map to a CCTV zone
         */
        virtual CctvZone getCctvZoneFromLocation( unsigned long locationKey ) = 0;


        /**
         * Gets the number of retries to send to the train when it times out.
         *
         * @return the number of times to resend the message
         */
        virtual unsigned long getTimsRetryLimit() = 0;


        /**
         * Gets the number of times to automatically reply to a TIMS request for reset message.
         * This applies to PA live broadcasts and ongoing PEC conversations.
         *
         * @return the number of times to automatically send a continue before prompting the operator
         */
        virtual unsigned long getTimsAutoContinueAckLimit() = 0;


        /**
         * Gets the default timeout for any messages sent to TIMS that dont have an explicit timeout set.
         *
         * @return the default timeout in milliseconds
         */
        virtual unsigned long getDefaultTimsMessageTimeout() = 0;


        /**
         * Gets the ISCS1 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerISCS1() = 0;


        /**
         * Gets the ISCS2 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerISCS2() = 0;


        /**
         * Gets the CCTV1 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerCCTV1() = 0;


        /**
         * Gets the VC1 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerVC1() = 0;


        /**
         * Gets the VC2 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerVC2() = 0;
		
        /**
         * Gets the PEC2 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerPEC2() = 0;

		/**
         * Gets the PEC3 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerPEC3() = 0;


        /**
         * Gets the PEC4 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerPEC4() = 0;

		/**
         * Gets the PEC5 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerPEC5() = 0;


        /**
         * Gets the PA1 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerPA1() = 0;


        /**
         * Gets the PA2 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerPA2() = 0;


        /**
         * Gets the PA3 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerPA3() = 0;


        /**
         * Gets the PA4 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerPA4() = 0;


        /**
         * Gets the TIS11 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerTIS1() = 0;


        /**
         * Gets the TIS2 timer value.
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getTimerTIS2() = 0;


        /**
         * Gets the OA1 timer value. This is the timer that is used to consider a train's OA1 invalid.
         * By default it is set to 3x OA1 period - which means a train must miss 3 consecutive OA1 tables for it to be considered invalid
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getOa1Timeout() = 0;


        /**
         * Gets the ATC timer value. This is the timer that is used to consider a train's ATC invalid.
         * By default it is set to 3x ATC period - which means a train must miss 3 consecutive ATC tables for it to be considered invalid
         *
         * @return the timeout in milliseconds
         */
        virtual unsigned long getAtcTimeout() = 0;


        /**
         * Gets the camera to activate for the given PEC.
         *
         * @return the camera to activate
         *
         * @exception InvalidParameterException if invalid PEC or CAR is given, or if the configuration is not valid.
         *
         * @param car    The Car the PEC is in. Cannot be All
         * @param pec    The PEC within the car. Cannot be All
         */
        virtual TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera getPecCamera( TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                                                         TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec ) = 0;


        /**
         * This will return the number of internal threads the agent can grow to.
         * The thread pool can dynamically grow to this limit as necessary.
         */
        virtual unsigned long getMaxThreadPoolSize() = 0;


		/**
		 * This will return a vector for a high priority depot location used for state synchronization
		 */
		virtual std::vector<unsigned long> getStateSyncLocPriority() = 0;


		/**
		 * This will return the status12 information
		 */
		virtual unsigned char getTrainStatus12() = 0;

    };

} // TA_IRS_App

#endif // !defined(ITrainAgentConfiguration_H)
