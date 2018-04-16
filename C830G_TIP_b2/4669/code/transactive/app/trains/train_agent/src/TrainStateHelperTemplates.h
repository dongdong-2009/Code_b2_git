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
#if !defined(TrainStateHelperTemplates_H)
#define TrainStateHelperTemplates_H

// Note this template class assumes all the train state types are pre-included
// only other specific stuff needs to be included here

namespace TA_IRS_App
{

    /**
     * This is a utility used by std::find to search train comms state for matching TSIs
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:35 PM
     */
    class FindTrainWithTsi : public std::unary_function< const TA_IRS_Bus::TrainIdToStateMap::value_type&, bool >
    {

    public:

        /**
         * Create the comparison function with the data to match in each given state.
         *
         * @param tsi    The TSI to match
         */
        FindTrainWithTsi( const std::string& tsi )
                : m_tsi( tsi )
        {
        }


        /**
         * Destructor
         */
        virtual ~FindTrainWithTsi() {}


        /**
         * the actual comparison function
         *
         * @return true if the state returns true for isThisTrain()
         *
         * @param state    This is the argument passed into the unary function - in this case a TrainStatePtr&
         */
        result_type operator()( argument_type state )
        {
            // Down cast the argument_type state (which is a TrainCommsStatePtr) to the specific type using boost_dynamic_pointer_cast.
            TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsStatePtr::element_type >( state.second );

            // Assert that the downcast was successful.
            TA_ASSERT( NULL != commsState.get(), "Invalid type given to search function" );

            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( commsState );

            return commsState->isThisTrain( m_tsi );
        }


    private:

        /**
         * Private default constructor
         */
        FindTrainWithTsi();


        /**
         * The TSI to match
         */
        std::string m_tsi;

    };


    /**
     * This is a utility used by std::for_each to search train state for matching calls.
     *
     * The specific type of the train state is given by the template - the state must have a getCallId and a getConsoleId function on it to work.
     *
     * Types this can be used with are:
     * TrainCallStatePtr
     * TrainPaStatePtr
     * TrainPecStatePtr
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:34 PM
     */

    template<typename SpecificStatePtr> class FindTrainsWithCall : public std::unary_function< TA_IRS_Bus::TrainIdToStateMap::value_type&, void >
    {

    public:

        /**
         * Create the comparison function with the data to match in each given state.
         *
         * @param consoleId    The console ID to match
         * @param callId    The call ID to match
         */
        FindTrainsWithCall( unsigned long consoleId,
                            unsigned long callId )
                : m_consoleId( consoleId ),
                  m_callId( callId )
        {
        }


        /**
         * Destructor
         */
        virtual ~FindTrainsWithCall() {}


        /**
         * the actual comparison function
         *
         * @return true if the call and console IDs match
         *
         * @param state    This is the argument passed into the unary function - in this case a TrainStatePtr&
         */
        result_type operator()( argument_type state )
        {
            // Down cast the argument_type state (which is a TrainStatePtr) to the specific type (SpecificState) using boost dynamic_pointer_cast.

            // note that VC6 is not compliant, it cant handle proper use of templates
#ifdef WIN32
            SpecificStatePtr specificState = boost::dynamic_pointer_cast< SpecificStatePtr::element_type >( state.second );
#else            
            SpecificStatePtr specificState = boost::dynamic_pointer_cast< typename SpecificStatePtr::element_type >( state.second );
#endif

            // Assert that the downcast was successful.
            TA_ASSERT( NULL != specificState.get(), "Invalid type given to search function" );

            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( specificState );

            // if the callId and consoldId in the members match the callId and consoleId in the state add the train to the list

            if ( ( specificState->getConsoleId() == m_consoleId ) &&
                 ( specificState->getCallId() == m_callId ) )
            {
                m_matchingTrains.insert( specificState->getTrainId() );
            }
        }


        /**
         * Gets the trains with the given call ID and console id in the state
         *
         * @return the list of trains
         */
        TA_IRS_Bus::CommonTypes::TrainIdList getTrains()
        {
            return m_matchingTrains;
        }


    private:

        /**
         * Private default constructor
         */
        FindTrainsWithCall();


        /**
         * The console ID to match
         */
        unsigned long m_consoleId;


        /**
         * The call ID to match
         */
        unsigned long m_callId;


        /**
         * The list of trains with the given call and console IDs
         */
        TA_IRS_Bus::CommonTypes::TrainIdList m_matchingTrains;

    };


    /**
     * This is a utility used by std::for_each to search train state for matching calls.
     *
     * The specific type of the train state is given by the template - the state must have a getCallId and a getConsoleId function on it to work.
     *
     * Types this can be used with are:
     * TrainCallStatePtr
     * TrainPaStatePtr
     * TrainPecStatePtr
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:34 PM
     */

    class FindTrainsWithAnnouncementId : public std::unary_function< TA_IRS_Bus::TrainIdToStateMap::value_type&, void >
    {

    public:

        /**
         * Create the comparison function with the data to match in each given state.
         *
         * @param announcementId    The announcement ID to match
         * @param matchContinueRequired    whether to match only those trains requiring a continue
         */
        FindTrainsWithAnnouncementId( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                      bool matchContinueRequired )
                : m_announcementId( announcementId ),
                  m_matchContinueRequired( matchContinueRequired )
        {
        }


        /**
         * Destructor
         */
        virtual ~FindTrainsWithAnnouncementId() {}


        /**
         * the actual comparison function
         *
         * @return true if the call and console IDs match
         *
         * @param state    This is the argument passed into the unary function - in this case a TrainStatePtr&
         */
        result_type operator()( argument_type state )
        {
            // Down cast the argument_type state (which is a TrainPaStatePtr) to the specific type using boost dynamic_pointer_cast.
            TrainPaStatePtr paState = boost::dynamic_pointer_cast< TrainPaStatePtr::element_type >( state.second );

            // Assert that the downcast was successful.
            TA_ASSERT( NULL != paState.get(), "Invalid type given to search function" );

            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( paState );

            if ( paState->getAnnouncementId() == m_announcementId )
            {
                // if it is required to match continue required
                if ( true == m_matchContinueRequired )
                {
                    // if a continue is required in the state
                    if ( true == paState->isContinueRequired() )
                    {
                        // add the train to the list
                        m_matchingTrains.insert( paState->getTrainId() );
                    }
                }
                else
                {
                    // add the train to the list
                    m_matchingTrains.insert( paState->getTrainId() );
                }
            }
        }


        /**
         * Gets the trains with the given call ID and console id in the state
         *
         * @return the list of trains
         */
        TA_IRS_Bus::CommonTypes::TrainIdList getTrains()
        {
            return m_matchingTrains;
        }


    private:

        /**
         * Private default constructor
         */
        FindTrainsWithAnnouncementId();


        /**
         * The announcement ID to match
         */
        TA_IRS_Bus::PaTypes::AnnouncementId m_announcementId;


        /**
         * The list of trains with the given call and console IDs
         */
        TA_IRS_Bus::CommonTypes::TrainIdList m_matchingTrains;


        /**
         * Whether to match only trains requiring continue
         */
        bool m_matchContinueRequired;

    };


    /**
     * This is a utility used by std::find to search train comms state for matching TSIs
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:35 PM
     */

    class FindCallForAnnouncement : public std::unary_function< TA_IRS_Bus::TrainIdToStateMap::value_type&, bool >
    {

    public:

        /**
         * Create the comparison function with the data to match in each given state.
         *
         * @param announcementId    The announcement ID to search for
         */
        FindCallForAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId )
                : m_announcementId( announcementId )
        {
        }


        /**
         * Destructor
         */
        virtual ~FindCallForAnnouncement() {}


        /**
         * the actual comparison function
         *
         * @return true if the state contains the correct announcement ID, and some call details.
         *
         * @param state    This is the argument passed into the unary function - in this case a TrainStatePtr&
         */
        result_type operator()( argument_type state )
        {
            // Down cast the argument_type state (which is a TrainPaStatePtr) to the specific type using boost dynamic_pointer_cast.
            TrainPaStatePtr paState = boost::dynamic_pointer_cast< TrainPaStatePtr::element_type >( state.second );

            // Assert that the downcast was successful.
            TA_ASSERT( NULL != paState.get(), "Invalid type given to search function" );

            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( paState );

            // if the m_announcementId matches the announcement ID in the state
            if ( paState->getAnnouncementId() == m_announcementId )
            {
                // if there is an ongoing call, then the matching record is found
                if ( ( 0 != paState->getConsoleId() ) &&
                     ( 0 != paState->getCallId() ) &&
                     ( false == paState->getSessionId().empty() ) )
                {
                    return true;
                }
            }

            return false;
        }


    private:

        /**
         * Private default constructor
         */
        FindCallForAnnouncement();


        /**
         * The announcement ID to match
         */
        TA_IRS_Bus::PaTypes::AnnouncementId m_announcementId;

    };


    /**
     * This is a utility used by std::find to search train comms state for matching TSIs
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:35 PM
     */

    class FindStaticGroupForAnnouncement : public std::unary_function< TA_IRS_Bus::TrainIdToStateMap::value_type&, bool >
    {

    public:

        /**
         * Create the comparison function with the data to match in each given state.
         *
         * @param announcementId    The announcement ID to search for
         */
        FindStaticGroupForAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId )
                : m_announcementId( announcementId )
        {
        }


        /**
         * Destructor
         */
        virtual ~FindStaticGroupForAnnouncement() {}


        /**
         * the actual comparison function
         *
         * @return true if the state contains the correct announcement ID, and some call details.
         *
         * @param state    This is the argument passed into the unary function - in this case a TrainStatePtr&
         */
        result_type operator()( argument_type state )
        {
            // Down cast the argument_type state (which is a TrainPaStatePtr) to the specific type using boost dynamic_pointer_cast.
            TrainPaStatePtr paState = boost::dynamic_pointer_cast< TrainPaStatePtr::element_type >( state.second );

            // Assert that the downcast was successful.
            TA_ASSERT( NULL != paState.get(), "Invalid type given to search function" );

            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( paState );

            // if the announcement ID matches the announcement ID in the state

            if ( paState->getAnnouncementId() == m_announcementId )
            {
                // get the Announcement Static Group
                std::string groupTsi = paState->getAnnouncementStaticGroup();

                // if it is not empty, then we have found the group tsi
                // if it is empty, keep looking
                return ( false == groupTsi.empty() );
            }

            return false;
        }


    private:

        /**
         * Private default constructor
         */
        FindStaticGroupForAnnouncement();


        /**
         * The announcement ID to match
         */
        TA_IRS_Bus::PaTypes::AnnouncementId m_announcementId;

    };


} // TA_IRS_App

#endif // !defined(TrainStateHelperTemplates_H)
