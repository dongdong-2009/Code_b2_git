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

#if !defined(PecTypes_H)
#define PecTypes_H

#include "bus/trains/TrainCommonLibrary/src/ProtocolPecTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include <string>
#include <vector>

namespace TA_IRS_Bus
{

    namespace PecTypes
    {

        /**
         * This indicates the update type for the given PEC on the given train.
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:24 PM
         */
        enum EPecUpdateType
        {
            /**
             * A new PEC has been activated, or an existing PEC has been updated. The banner
             * should refresh the information with the new information in this message. If the
             * PEC is not in the banner it should be updated.
             */
            PECUpdated = 1,
            /**
             * The given PEC has been reset, a single reset message will be sent for each PEC
             * on a train. So if a train has multiple PEC, and a PEC reset all has occurred, a
             * separate message will be sent for each PEC.
             */
            PECReset = 2,
            /**
             * The given PEC has been reset, due to MPU changeover.
             */
            PECMpuChangeover = 3,
            /**
             * The given PEC is in progress, but the audio call has been lost. This is simply
             * used by the PEC dialog open for the given PEC to notify the operator that this
             * has occurred.
             */
            PECCallLost = 4,
			/**
			 * A given PEC has been Hold.
			 */
			PECHold = 5
        };


        /**
         * The current assignment of a PEC, unanswered, answered by driver, or answered by
         * operator.
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:24 PM
         */
        enum EPecSource
        {
            PecSourceUnassigned = 0,
            PecSourceDriver = 1,
            PecSourceOperator = 2
        };

		
		/**
         * The current assignment for the PEC Train Source
         * @author Lucky Cuizon
         * @created 19-April-2011 03:20:27 PM
         */
        enum EPecTrainSource
        {
            LocalTrain = 0,
			RemoteTrain = 1
        };


        /**
         * Holds the details of an activated PEC button.
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:24 PM
         */
        struct PecPoint
        {

            /**
             * The car the pec was activated in.
             */
            ProtocolPecTypes::EPecCarNumber car;

            /**
             * The PEC within the car that was activated.
             */
            ProtocolPecTypes::EPecNumber pec;

            /**
             * Whether the PEC is valid or has been invalidated
             */
            bool valid;

            /**
             * Whether the pec is answered and if so by who
             */
            EPecSource source;

			/**
             * Use to determine the PEC Train Source
             */
            EPecTrainSource pecTrainSource;


            /**
             * The time this PEC was first activated
             */
            time_t creationTime;

            /**
             * compares 2 pec points without taking timestamp into account.
             */
            bool operator<( const PecPoint& rhs ) const;

            /**
             * compares 2 pec points without taking timestamp into account.
             */
            bool operator==( const PecPoint& rhs ) const;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;

        };


        /**
         * Represents a set of active PEC points, points are unique in the set.
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:24 PM
         */
        typedef std::set< PecPoint > ActivePecPointList;


        /**
         * PEC Notification Message
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:24 PM
         */
        struct PecUpdate
        {

            /**
             * The train this applies to
             */
            CommonTypes::TrainIdType trainId;


            /**
             * The type of notification this is
             */
            EPecUpdateType updateType;

            /**
             * The PEC this notification is for, and its updated details.
             */
            PecPoint pec;

            /**
             * Only set when pec is "in progress" by an operator. Used to determine whether
             * caller or another operator is in control of PEC
             */
            std::string sessionId;


            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;

			/**
			 * PrimaryTsi
			 */
			unsigned long primaryTsi;
			/**
			 * Secondary Tsi
			 */
			unsigned long secondaryTsi;


        };


        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:24 PM
         */
        typedef std::vector< PecUpdate > PecUpdateList;


        /**
         * Utility functions to print simple types to string
         */
        std::string pecUpdateTypeToString( EPecUpdateType pecUpdateType );
        std::string pecSourceToString( EPecSource pecSource );
        std::string activePecPointListToString( const ActivePecPointList& activePecPointList );
        std::string pecUpdateListToString( const PecUpdateList& pecUpdateList );
		std::string pecTrainSourceToString( EPecTrainSource pecTrainSource );

    }

}

#endif // !defined(PecTypes_H)
