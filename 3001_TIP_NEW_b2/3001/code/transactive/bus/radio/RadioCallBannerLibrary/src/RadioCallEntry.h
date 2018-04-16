/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/radio/RadioCallBannerLibrary/src/RadioCallEntry.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/06/20 14:30:32 $
  * Last modified by:  $Author: peter.wong $
  * 
  * This class extends AbstractCallEntry to provide the implementation of
  * a radio call entry.
  * When activated, it opens the radio manager with the corresponding
  * communication selected.
  */

#ifndef CALL_BANNER_RADIO_CALL_ENTRY_H
#define CALL_BANNER_RADIO_CALL_ENTRY_H

#include "bus/banner_pages/call_page_library/src/AbstractCallEntry.h"
#include "bus/radio/RadioCallBannerLibrary/src/RadioCallFactory.h"

// krtc
#include "bus/radio/radio_agent/idl/src/RadioMFTAgentCallbackIDL.h"


namespace TA_IRS_Bus
{
    /**
     * @version 1.0
     * @created 11-Feb-2004 03:38:50 PM
     */
    class RadioCallEntry : public TA_IRS_Bus::AbstractCallEntry 
    {
    
    public:
        
        // The type of call entry this is, ie Radio
        static const std::string CALL_ENTRY_TYPE;

       
        /**
         *  Creates a new call entry.
         *
         *  @param id          the id of this Radio call entry.
         *  @param details     the details of the notification that caused
         *                     this entry to be created.
         */
        RadioCallEntry(
            unsigned long id,
            const TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent& details);

        /**
         *  The destructor.
         */
        virtual ~RadioCallEntry();

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
         * @return true if this is an emergency call
         */
        bool isEmergency() const;

        /**
         *  This method is called when the call entry is activated, for
         *  example when the user double-clicks on it.
         */
        virtual void onActivate();

		void updateStatus(Radio::EPrivateCallState status);



    private:
        RadioCallEntry();
        RadioCallEntry(const RadioCallEntry& theRadioCallEntry);


        // The details of the call event
        TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent m_details;
		Radio::EPrivateCallState m_status;
    };

}// TA_Bus

#endif // CALL_BANNER_RADIO_CALL_ENTRY_H
