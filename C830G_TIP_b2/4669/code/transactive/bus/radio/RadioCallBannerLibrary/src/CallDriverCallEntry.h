/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: PRIV_4669_TRAIN_ECP/CODE_4669/transactive/bus/radio/RadioCallBannerLibrary/src/CallDriverCallEntry.h $
  * @author:  oliverk
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(CallDriverCallEntry_H)
#define CallDriverCallEntry_H


#include "bus/banner_pages/call_page_library/src/AbstractCallEntry.h"

#include "bus/trains/TrainCommonLibrary/src/CallTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"


namespace TA_IRS_Bus
{
    /**
     * @version 1.0
     * @created 11-Feb-2004 03:38:50 PM
     * @author oliverk
     * @version 1.0
     */
    class CallDriverCallEntry : public TA_IRS_Bus::AbstractCallEntry
    {

    public:


        /**
         * Creates a new call entry.
         *
         * @param id          the id of this call entry
         * @param occCall     the call details
         * @param details     the details of the train
         */
        CallDriverCallEntry( unsigned long id,
                             const CallTypes::OccCallRequest& occCall,
                             const TrainInformationTypes::TrainDetails& trainDetails );


        /**
         * Destructor
         */
        virtual ~CallDriverCallEntry();


        /**
         * Gets the call type
         *
         * @return the call type
         */
        static std::string getStaticType();


        /**
         * Gets the call category
         *
         * @return the call category
         */
        virtual std::string getCategory() const;


        /**
         * Gets the call type
         *
         * @return the call type
         */
        virtual std::string getType() const;


        /**
         * Uses the train details to return the location string
         *
         * @return the location of the call
         */
        virtual std::string getLocationStr() const;


        /**
         * Gets the severity of the call.
         *
         * @return the call severity
         */
        virtual int getSeverity() const;


        /**
         * Gets the status of the call entry.
         * For display in the banner status column.
         *
         * @return the status string
         */
        virtual std::string getStatusStr() const;


        /**
         * Gets the time the call was registered
         *
         * @return the incoming call time
         */
        virtual time_t getCreateTime() const;


        /**
         * Checks if this call is emergency or not.
         *
         * @return true if this is an emergency call
         */
        virtual bool isEmergency() const;


        /**
         * This method is called when the call entry is activated, for
         * example when the user double-clicks on it.
         */
        virtual void onActivate();


        /**
         * Updates the details of the call.
         * This is for updates to existing call entries
         *
         * @param details     the details of incoming OCC Call
         */
        void updateDetails( const CallTypes::OccCallRequest& occCall );


        /**
         * Updates the details of the call.
         * This is for updates to existing call entries
         *
         * @param trainDetails     the details of the train
         */
        void updateDetails( const TrainInformationTypes::TrainDetails& trainDetails );
        

    private:


        /**
         * Private Constructor
         */
        CallDriverCallEntry();
        CallDriverCallEntry( const CallDriverCallEntry& theCallDriverCallEntry );


        /**
         * Gets the source string to display in the banner
         * 
         * @param occCall the call to get the information for
         * @param trainDetails the details about the train
         *
         * @return the call source string
         */
        std::string getSource( const CallTypes::OccCallRequest& occCall,
                               const TrainInformationTypes::TrainDetails& trainDetails ) const;


        /**
         * train call type
         */
        static const std::string DRIVER_ENTRY_TYPE;


        /**
         * Call status
         */
        static const std::string CALL_STATUS_STRING;


        /**
         * Call information
         */
        static const std::string CALL_INFORMATION_STRING;


        /**
         * The details of the incoming train call
         */
        CallTypes::OccCallRequest m_occCall;


        /**
         * The details of the train the call is from
         */
        TrainInformationTypes::TrainDetails m_trainDetails;
    };

}

#endif // !defined(CallDriverCallEntry_H)
