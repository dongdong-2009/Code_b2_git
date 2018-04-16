#ifndef IDISPLAY_REQUEST_H_INCLUDED
#define IDISPLAY_REQUEST_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/IDisplayRequest.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This interface contains the database retrieval and write methods
  * for Display Requests for the Traveller Information System (TIS) Agent.
  */

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

#include <vector>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

namespace TA_Base_Core
{
    class IData;
}

namespace TA_IRS_Core
{
    class IDisplayRequest
    {
    public:

        enum EMessageType
	    {
		    PREDEFINED = 0,
		    ADHOC = 1,
			RATIS_IN_NEW = 2,
			RATIS_OUT_NEW = 3,
			RATIS_IN_UPDATE = 4,
			RATIS_OUT_UPDATE = 5,
			RATIS_IN_CLEAR = 6,
			RATIS_OUT_CLEAR = 7
        };

        /**
         * ~ICameraPresets
         *
         * Standard destructor.
         */

        virtual ~IDisplayRequest() {};

		// Not sure I need this one

        /**
         * getKey
         *
         * 
         *
         * @return      std::string
         *              The name of the preset.
         *
         * @param       unsigned short presetIndex
         *              The index of the preset.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if:
         *              1. The preset index was not found.
         *              2. The data cannot be converted to the correct format.
         */

        virtual std::string getKey() = 0;
		virtual std::string getTimestamp() = 0;
		virtual std::string getDestinationSystem() = 0;
		virtual std::string getSubmittedBy() = 0;
		virtual int getMessageType() = 0;
		virtual int getMessageTag() = 0;
		virtual int getLibrarySection() = 0;
		virtual unsigned short getLibraryVersion() = 0;
		virtual std::string getAdhocMessageContent() = 0;
		virtual std::string getStartTime() = 0;
		virtual std::string getEndTime() = 0;
		virtual bool getContinuous() = 0;
		virtual int getPriority() = 0;
		virtual std::string getRatisTag() = 0;
		virtual std::string getRatisDestination() = 0;
		virtual bool getRatisOverridable() = 0;
		virtual std::string getStisDestinationStation() = 0;
		virtual std::string getStisDestinationLevels() = 0;
		virtual std::string getStisDestinationPids() = 0;
		virtual std::string getTtisDestinationTrainId() = 0;
		virtual std::string getTtisDestinationPids() = 0;
		virtual time_t getCreatedTime() = 0;
		//hongran++ TD17502
		virtual TA_Base_Core::IfSendSuccess getIfSendSuccess() = 0;
		//++hongran TD17502

        virtual TA_Base_Core::DisplayAttributes getDisplayAttributes() = 0;
        virtual TA_Base_Core::PlasmaAttributes getPlasmaAttributes() = 0;
        virtual TA_Base_Core::LEDAttributes getLEDAttributes() = 0;

		virtual void setTimestamp( const std::string& timestamp ) = 0;
		virtual void setDestinationSystem( const std::string& destinationSystem ) = 0;
		virtual void setSubmittedBy( const std::string& submittedBy ) = 0;
		virtual void setMessageType( int messageType ) = 0;
		virtual void setMessageTag( int messageTag ) = 0;
		virtual void setLibrarySection( int librarySection ) = 0;
		virtual void setLibraryVersion( unsigned short libraryVersion ) = 0;
		virtual void setAdhocMessageContent( const std::string& adhocMessageContent ) = 0;
		virtual void setStartTime( const std::string& startTime ) = 0;
		virtual void setEndTime( const std::string& endTime ) = 0;
		virtual void setContinuous( bool continuous ) = 0;
		virtual void setPriority( int continuous ) = 0;
		virtual void setRatisTag( const std::string& ratisTag ) = 0;
		virtual void setRatisDestination( const std::string& ratisTag ) = 0;
		virtual void setRatisOverridable( bool overridable ) = 0;
		virtual void setStisDestinationStation( const std::string& destinationStation ) = 0;
		virtual void setStisDestinationLevels( const std::string& destinationLevels ) = 0;
		virtual void setStisDestinationPids( const std::string& destinationPids ) = 0;
		virtual void setTtisDestinationTrainId( const std::string& destinationTrainId ) = 0;
		virtual void setTtisDestinationPids( const std::string& destinationPids ) = 0;

        virtual void setDisplayAttributes( TA_Base_Core::DisplayAttributes attributes ) = 0;
        virtual void setPlasmaAttributes( TA_Base_Core::PlasmaAttributes attributes ) = 0;
        virtual void setLEDAttributes( TA_Base_Core::LEDAttributes attributes ) = 0;

		//hongran++ TD17502
		virtual void setIfSendSuccess(TA_Base_Core::IfSendSuccess successful) = 0;
		//++hongran TD17502

        /**
          * invalidate
          *
          * Marks the data contained by this object as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */

        virtual void invalidate() = 0;

		virtual void applyChanges() = 0;

        virtual void populateData( boost::shared_ptr< TA_Base_Core::IData > data, unsigned long index ) = 0;
    };

} //end namespace TA_IRS_Core

#endif 
