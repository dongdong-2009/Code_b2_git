#ifndef RATIS_MESSAGE_HELPER_H_INCLUDED
#define RATIS_MESSAGE_HELPER_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/RATISMessageHelper.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * RATISMessageHelper contains the get and set methods that do the 
  * actual database work for the RATISMessageData class.
  * It accesses the ti_ratis_message table.
  */

#pragma warning ( disable : 4786 )

#include <map>
#include <climits>

#include "core/data_access_interface/tis_agent/src/IRATISMessage.h"


namespace TA_IRS_Core
{
    class RATISMessageHelper
    {
    public:
        
		RATISMessageHelper();

       /**
         * RATISMessageHelper
         * 
         * This constructor creates a new DisplayRequestHelper for the specified key.
         *
         * @param       unsigned long displayRequestPKey
         *              The database key for the DisplayRequest parent of this helper.
         */

        RATISMessageHelper( unsigned long ratisMessagePKey );

        /**
          * RATISMessageHelper
          *
          * Standard copy constructor.
          */

        RATISMessageHelper( const RATISMessageHelper& theRATISMessageHelper );

        /**
         * ~RATISMessageHelper
         *
         * Standard destructor.
         */

        virtual ~RATISMessageHelper() {};

        /**
         * getKey
         *
         * Returns the DisplayRequest key for this DisplayRequest.
         *
         * Preconditions:   Either - this DisplayRequest entry was initially 
         *                           loaded from the database
         *                  OR     - writeDisplayRequestData() has already been called
         *
         * @return      unsigned long
         *              The key for this VideoOutputGroup.
         */

        unsigned long getKey();
		long getSessionRef();
		std::string getStartTime();
		std::string getEndTime();
		bool getRequiresVetting();
		bool getOverridable();
		TA_Base_Core::ERATISMessageType getType();  
		TA_Base_Core::ERATISMessageStatus getStatus();			
		long getCreationDate();
		std::string getMessageText();
		int getPriority();
		std::string getTag();
		std::string getDestination();

		void setSessionRef( long sessionRef );
		void setStartTime( const std::string& startTime );
		void setEndTime( const std::string& endTime );
		void setRequiresVetting( bool requiresVetting );
		void setOverridable( bool overridable );
		void setType( TA_Base_Core::ERATISMessageType type ); 
		void setStatus( TA_Base_Core::ERATISMessageStatus status );		
		void setCreationDate( long creationDate );
		void setMessageText( const std::string& messageText );
		void setPriority( int priority );
		void setTag( const std::string& tag );
		void setDestination( const std::string& destination );

		bool isNew() const { return m_isNew; };

		bool hasChanged() {return m_hasChanged;}

 		void addNewRATISMessageData();

		void writeRATISMessageData();

		void updateExistingRATISMessageData();

		void deleteThisRATISMessage();

        /**
         * invalidate
         *
         * Marks the data contained by this RATIS Message as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    Either - this RATIS Message was initially loaded from the database
         *                  OR     - writeRATISMessageData() has already been called, as it does 
         *                           not make any sense to invalidate a RATIS Message that has not 
         *                           yet been written to the database.
         */

	    void invalidate();



    private:
        //
        // Disable the assignment operator.
        //

		RATISMessageHelper& operator=( const RATISMessageHelper& );
        
        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * Precondition:    Either - this DisplayRequest was initially loaded from the database
         *                  OR     - writeDisplayRequestData() has already been called.
         */

	    void reload();

         //
        // Flag to indicate if the data needs reloading.
        //

	    bool m_isValidData;

        //
        // Flag to indicate if this is a new RATISMessage or an existing one
        // loaded from the database.
        //

        bool m_isNew;

		bool m_hasChanged;
        
        //
        // The RATISMessage database primary key 
        //
        unsigned long m_key;

		long m_sessionRef;
		std::string m_startTime;
		std::string m_endTime;
		bool m_requiresVetting;
		bool m_overridable;
		TA_Base_Core::ERATISMessageType  m_type;
		TA_Base_Core::ERATISMessageStatus  m_status;
		long m_creationDate;
		std::string m_messageText;
		int  m_priority;
		std::string m_tag;
		std::string m_destination;

    };

} // closes TA_Base_Core

#endif 
