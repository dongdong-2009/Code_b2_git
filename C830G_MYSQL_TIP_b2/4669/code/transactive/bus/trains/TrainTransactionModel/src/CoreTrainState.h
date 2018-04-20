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

#if !defined(CoreTrainState_H)
#define CoreTrainState_H


#include "bus/trains/TrainTransactionModel/src/TrainState.h"

#include <string>


namespace TA_IRS_Bus
{


    /**
     * This is the core data for a train. It contains the minimum amount of
     * information required to communicate with a train.
     *
     * This data must be kept up to date by the protocol specific component of the
     * train agent. It must also be added before any other state objects for this
     * train.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:36 PM
     */

    class CoreTrainState : public TrainState
    {

    public:


        /**
         * Creates the object with a train ID, validity flag, radio ID and location key.
         *
         * The validity is false unless all parameters are valid (ie not default values)
         *
         * @param trainId
         * @param radioId    This is the radio number used to call the train.
         * @param location    The location of this train.
         */
        CoreTrainState( CommonTypes::TrainIdType trainId,
                        const std::string& radioId = "",
                        unsigned long location = 0 );


        /**
         * Destrutor
         */
        virtual ~CoreTrainState();


        /**
         * Returns the train resource this state represents.
         */
        virtual CommonTypes::ETrainResource getResourceCategory() const;


        /**
         * Gets whether the train is a valid train.
         */
        virtual bool isValid() const;


        /**
         * Gets the ID used to contact the train.
         */
        virtual std::string getRadioId() const;


        /**
         * Gets the trains location key.
         */
        virtual unsigned long getLocation() const;


        /**
         * Sets the trains validity
         *
         * @param isValid    The validity flag (ie is this train present in the signalling system)
         */
        virtual void setValidity( bool isValid );


        /**
         * Sets the ID used to contact the train
         *
         * @param radioId the new radio ID
         */
        virtual void setRadioId( const std::string& radioId );


        /**
         * Sets the trains location key
         *
         * @param locationKey the new location key
         */
        virtual void setLocation( unsigned long locationKey );


    private:


        /**
         * Construtor
         */
        CoreTrainState();

        /**
         * This is the resource category for this state object. It isnt really a resource
         * but comes from other resources, so -1 is reserved for the CoreTrainState.
         */
        static const CommonTypes::ETrainResource RESOURCE_CATEGORY;


        /**
         * Whether this train is valid in signalling or not. A trains details are never
         * deleted, rather they are marked valid or invalid.
         */
        bool m_isValid;


        /**
         * This is the radio number used to communicate with the train via radio.
         */
        std::string m_radioId;


        /**
         * This is the location of the train in signalling.
         */
        unsigned long m_currentLocation;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:36 PM
     */
    typedef boost::shared_ptr< CoreTrainState > CoreTrainStatePtr;

}

#endif // !defined(CoreTrainState_H)
