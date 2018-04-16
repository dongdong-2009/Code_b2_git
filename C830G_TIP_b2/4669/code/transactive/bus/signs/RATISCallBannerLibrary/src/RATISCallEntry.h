/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/signs/RATISCallBannerLibrary/src/RATISCallEntry.h $
  * @author:  Rob Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class extends AbstractCallEntry to provide the implementation of
  * a RATIS 'call' entry.
  * When activated, it opens the RATIS approval manager with the corresponding
  * incoming RATIS message selected.
  */

#ifndef CALL_BANNER_RATIS_CALL_ENTRY_H
#define CALL_BANNER_RATIS_CALL_ENTRY_H

#include "bus/banner_pages/call_page_library/src/AbstractCallEntry.h"
#include "bus/signs_4669/TISManagerCorbaDef/src/ISTISManagerCorbaDef.h"
#include "bus/signs/RATISCallBannerLibrary/src/RATISCallFactory.h"

namespace TA_Base_Bus
{
    class RightsLibrary;
}

namespace TA_IRS_Bus
{
    class RatisApprovalDialog; // Forward declaration

    class RATISCallEntry : public TA_IRS_Bus::AbstractCallEntry 
    {
    
    public:
        enum EPriorityGroup
        {
            EMERGENCY,
            NORMAL,
            PRIORITY6
        };

        
        // The type of call entry this is, ie Radio
        static const std::string CALL_ENTRY_TYPE;
        
        /**
         *  Creates a new call entry.
         *
         *  @param id          the id of this Radio call entry.
         *  @param details     the details of the notification that caused
         *                     this entry to be created.
         */
        RATISCallEntry(
            unsigned long id,
            const TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails& details,
            RATISCallFactory& factory);


        /**
         *  The destructor.
         */
        virtual ~RATISCallEntry();

        /**
         * Pure virtual implementations (see AbstractCallEntry for more details)
         */
        virtual std::string getLocationStr() const;
        virtual std::string getStatusStr() const;
        virtual time_t getCreateTime() const;
        virtual int getSeverity() const;
        virtual std::string getCategory() const { return CALL_ENTRY_TYPE; }

        /**
         *  Gets the type of call entry that this object represents, as a
         *  string.
         */
        virtual std::string getType() const;

        /**
         *  This method is called when the call entry is activated, for
         *  example when the user double-clicks on it.
         */
        virtual void onActivate();


        // Methods to retrieve the fields out of the RATISMessageDetails struct.
        virtual std::string getRATISTag() const;
        virtual TA_Base_Core::ERATISMessageType getRATISType() const;
        virtual std::string getRATISTypeAsString() const;
        virtual std::string getStartTime() const;
        virtual std::string getEndTime() const;
        virtual std::string getDestination() const;
        virtual int getPriority() const;
        bool isOverridable() const;
        bool isTTIS() const;
        bool isClearRequest() const;


        /** 
          * update
          *
          * Updates the entry with new details
          *
          * @param details The new details
          */
        void update(const TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails& details);


        /** 
          * updateStatus
          *
          * Update the status of the entry
          *
          * @param status The new status
          */
        void updateStatus(TA_Base_Core::ERATISMessageStatus status);


        /** 
          * getMessageContentMaximumLength
          *
          * Returns the maximum length of the message content
          *
          *
          * @return the maximum length of the message content
          */
        int getMessageContentMaximumLength() const;


        /** 
          * getPriorityGroup
          *
          * Returns the priority group based that this priority falls upon
          *
          * @param priority 
          *
          * @return the priority group based that this priority falls upon
          */
        EPriorityGroup getPriorityGroup(int priority) const;


        /**
         * submitRATISVettingResponse
         *
         * Submits a response for this message saying whether or not it was
         * approved.
         *
         * @param approved  True if the message is approved, and false otherwise.
         * @param priority The priority level of the message.
         * @param content The content of the message.
         */
        virtual void submitRATISVettingResponse(bool approved, const unsigned short priority = 0, const char* content = NULL);

    protected:
        
        /**
         *  Shows a message box indicating an error has occurred.
         *  The dialog must exist before this method is called or
         *  it will assert.
         */
        void showErrorMessage(const std::string& errorMessage);
        
    private:

        /**
         * Internal helper function to display details of caught exceptions (from the catch macro)
         * @param prefixMessage defines general description of what was happening at time of exception
         * @param exceptionName description of the type of exception thrown
         * @param exceptionDetails defines any additional exception information (if available)
         */
        void showExceptionCatchDetails(const char* prefixMessage, const char* exceptionName, const char* exceptionDetails);

        /** 
          * formatDate
          *
          * Change the specified timeStr from YYYYMMDDHHMISS to DD-MM-YYYY HH24:MI:SS 
          *
          * @param timeStr The time string
          *
          * @return the time string in the format of DD-MM-YYYY HH24:MI:SS 
          */
        std::string formatDate(const std::string& timeStr) const;


        RATISCallEntry();
        RATISCallEntry(const RATISCallEntry& theRATISCallEntry);

        // The dialog that is opened when the onActivate method is called.
        // There is only one copy for all RATIS call entries.
        static RatisApprovalDialog* s_dialog;

        // The details of the incoming RATIS message
        TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails m_details;

        // A reference to the call factory that created this call entry.
        RATISCallFactory& m_RATISCallFactory;


        static const std::string ALL_STATIONS;
        static const std::string ALL_TRAINS;
        static const std::string TRAIN;

        TA_Base_Bus::RightsLibrary* m_rightsMgr;

        unsigned int m_resourceKey;
    };

}// TA_Bus

#endif // CALL_BANNER_RATIS_CALL_ENTRY_H
