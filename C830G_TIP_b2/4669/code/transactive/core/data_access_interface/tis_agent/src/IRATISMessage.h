#ifndef IRATIS_MESSAGE_H_INCLUDED
#define IRATIS_MESSAGE_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/IRATISMessage.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This interface contains the database retrieval and write methods
  * for incoming RATIS messages for the Traveller Information System (TIS) Agent.
  */

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

#include <vector>
#include <string>
#include <map>

namespace TA_IRS_Core
{
    class IRATISMessage
    {
    public:

        /**
         * ~IRATISMessage
         *
         * Standard destructor.
         */

        virtual ~IRATISMessage() {};

        virtual unsigned long getKey() = 0;

		virtual long getSessionRef() = 0;
		virtual std::string getStartTime() = 0;
		virtual std::string getEndTime() = 0;
		virtual bool getRequiresVetting() = 0;
		virtual bool getOverridable() = 0;
		virtual TA_Base_Core::ERATISMessageType getType() = 0;  
		virtual TA_Base_Core::ERATISMessageStatus getStatus() = 0;			
		virtual long getCreationDate() = 0;
		virtual std::string getMessageText() = 0;
		virtual int getPriority() = 0;
		virtual std::string getTag() = 0;
		virtual std::string getDestination() = 0;
        virtual bool isForTTIS() = 0;
        virtual bool isForAllTrains() = 0;
        virtual std::vector<unsigned char> getDestinationIds() = 0;

		virtual void setSessionRef( long sessionRef ) = 0;
		virtual void setStartTime( const std::string& startTime ) = 0;
		virtual void setEndTime( const std::string& endTime ) = 0;
		virtual void setRequiresVetting( bool requiresVetting ) = 0;
		virtual void setOverridable( bool overridable ) = 0;
		virtual void setType( TA_Base_Core::ERATISMessageType type ) = 0;  
		virtual void setStatus( TA_Base_Core::ERATISMessageStatus status ) = 0;		
		virtual void setCreationDate( long creationDate ) = 0;
		virtual void setMessageText( const std::string& messageText ) = 0;
		virtual void setPriority( int priority ) = 0;
		virtual void setTag( const std::string& tag ) = 0;
		virtual void setDestination( const std::string& destination ) = 0;
		virtual void deleteThisRATISMessage() = 0;

        /**
          * invalidate
          *
          * Marks the data contained by this object as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */

        virtual void invalidate() = 0;

		virtual void applyChanges() = 0;

    };

} //end namespace TA_IRS_Core

#endif 
