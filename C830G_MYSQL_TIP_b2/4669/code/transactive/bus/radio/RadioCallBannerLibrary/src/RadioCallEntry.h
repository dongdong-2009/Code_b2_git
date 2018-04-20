/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: PRIV_4669_TRAIN_ECP/CODE_4669/transactive/bus/radio/RadioCallBannerLibrary/src/RadioCallEntry.h $
  * @author:  oliverk
  * @version: $Revision: #1 $
  *
  * Last modification: $Date: 2008/11/28 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(RadioCallEntry_H)
#define RadioCallEntry_H


#include "bus/banner_pages/call_page_library/src/AbstractCallEntry.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"


// forward declarations
namespace TA_IRS_Bus
{
    class RadioCallFactory;
}


namespace TA_IRS_Bus
{
    /**
     * @version 1.0
     * @created 11-Feb-2004 03:38:50 PM
     * @author oliverk
     * @version 1.0
     */

    class RadioCallEntry : public TA_IRS_Bus::AbstractCallEntry
    {

    public:


        /**
         * The type of call entry this is, Radio
         */
        static const std::string CALL_ENTRY_TYPE;


        /**
         * The type of call entry this is, SDS
         */
        static const std::string SDS_ENTRY_TYPE;


        /**
         * Creates a new call entry.
         *
         * @param id  the id of this Radio call entry.
         * @param consoleId The console the call is for
         * @param details The details of the call
         * @param radioCallFactory The radio call factory (used to get access to the radio agent)
         */
        RadioCallEntry( unsigned long id,
			            unsigned long consoleId,
                        const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& details,
                        RadioCallFactory& radioCallFactory );


        /**
         *  The destructor.
         */
        virtual ~RadioCallEntry();


        /**
         * Get the status to display on the banner
         *
         * @param status  The call status
         *
         * @return the call status string
         */
        static std::string getCallStatusAsString( TA_Base_Bus::IRadioCorbaDef::ECallStatusType status );


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
         * @param details     the updated call details
         */
        void updateDetails( const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& details );


    private:


        /** 
         * Private Constructor
         */
        RadioCallEntry();
        RadioCallEntry( const RadioCallEntry& theRadioCallEntry );


        /**
         * Get the information to display on the banner
         *
         * @param consoleId The console ID of this console
         * @param call  The call details
         * @param radioCallFactory  The call factory (used to access the agent)
         *
         * @return the information string
         */
        std::string getCallInformation( unsigned long consoleId,
                                        const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& call,
                                        RadioCallFactory& radioCallFactory ) const;


        /**
         * Get the source to display on the banner
         *
         * @param call  The call details
         *
         * @return the call source string
         */
        std::string getCallSourceAsString( const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& call ) const;


        /**
         * The details of the radio request
         */
        TA_Base_Bus::IRadioCorbaDef::CallDetailsType m_details;


        /**
         * The console key of the current console
         */
        unsigned long m_consoleId;


        /**
         * The call factory (used to get access to the agent)
         */
        RadioCallFactory& m_radioCallFactory;

    };

}

#endif // !defined(RadioCallEntry_H)
