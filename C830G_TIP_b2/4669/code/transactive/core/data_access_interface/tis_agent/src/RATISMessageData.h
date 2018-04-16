#ifndef RATIS_MESSAGE_DATA_H_INCLUDED
#define RATIS_MESSAGE_DATA_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/RATISMessageData.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class contains the database retrievals and writes for RATIS messages
  * for the Traveller Information System (TIS) Agent.
  * It implements the IRATISMessage interface.
  */

#include <map>
#include "core/data_access_interface/tis_agent/src/IRATISMessage.h"
#include "core/data_access_interface/tis_agent/src/RATISMessageHelper.h"


namespace TA_IRS_Core
{

    class RATISMessageData : public IRATISMessage
    {
    public:

		RATISMessageData();

        /**
         * RATISMessageData
         *
         * Standard constructor.
         *
         * @param       
         */

        RATISMessageData( unsigned long pkey );


        /**
         * ~RATISMessageData
         *
         * Standard destructor.
         */

        virtual ~RATISMessageData();
        
		virtual unsigned long getKey();

		virtual long getSessionRef();
		virtual std::string getStartTime();
		virtual std::string getEndTime();
		virtual bool getRequiresVetting();
		virtual bool getOverridable();
		virtual TA_Base_Core::ERATISMessageType getType();               // TODO - enum
		virtual TA_Base_Core::ERATISMessageStatus getStatus();			 // TODO - enum
		virtual long getCreationDate();
		virtual std::string getMessageText();
		virtual int getPriority();
		virtual std::string getTag();
		virtual std::string getDestination();
        virtual bool isForTTIS();
        virtual bool isForAllTrains();
        virtual std::vector<unsigned char> getDestinationIds();

		virtual void setSessionRef( long sessionRef );
		virtual void setStartTime( const std::string& startTime );
		virtual void setEndTime( const std::string& endTime );
		virtual void setRequiresVetting( bool requiresVetting );
		virtual void setOverridable( bool overridable );
		virtual void setType( TA_Base_Core::ERATISMessageType type );               // TODO - enum
		virtual void setStatus( TA_Base_Core::ERATISMessageStatus status );			 // TODO - enum
		virtual void setCreationDate( long creationDate );
		virtual void setMessageText( const std::string& messageText );
		virtual void setPriority( int priority );
		virtual void setTag( const std::string& tag );
		virtual void setDestination( const std::string& destination );

        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew();

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception TA_Base_Core::DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Item object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This item has not been deleted
         */
        virtual void applyChanges();

		/**
		  * invalidate
		  *
		  * Marks the data contained by this object as invalid. The next call to get...() 
		  * following a call to invalidate() will cause all the data to be reloaded from
		  * the database.
		  */

		virtual void invalidate();

        /** 
          * deleteThisRATISMessage
          *
          * Deletes this message from the database
          *
          */
		virtual void deleteThisRATISMessage();

    private:
        //
        // The copy constructor and assignment operator aren't needed so they are made
        // private here to hide them and prevent their use.
        //
        RATISMessageData& operator=( const RATISMessageData& );
        RATISMessageData( const RATISMessageData& );

		RATISMessageHelper* m_ratisMessageHelper;

        static const int STATION_SIZE;
        static const int STATION_START_INDEX;
        static const int STATION_END_INDEX;
        static const int ID_SIZE;
        static const int NUM_ID_SIZE;
        static const int NUM_ID_END;
        static const int STIS_MESSAGE_MAX_LENGTH;
        static const int TTIS_MESSAGE_MAX_LENGTH;

    };

} //end namespace TA_IRS_Core

#endif 

