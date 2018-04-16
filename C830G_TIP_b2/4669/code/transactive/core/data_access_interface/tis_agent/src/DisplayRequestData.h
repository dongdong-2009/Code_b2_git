#ifndef DISPLAY_REQUEST_DATA_H_INCLUDED
#define DISPLAY_REQUEST_DATA_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/DisplayRequestData.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class contains the database retrievals and writes for Display Requests
  * and their associated schedule entries, for the Traveller Information System (TIS) Agent.
  * It implements the IDisplayRequest interface.
  */

#include <map>
#include "core/data_access_interface/tis_agent/src/IDisplayRequest.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestData.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestHelper.h"



namespace TA_IRS_Core
{

    class DisplayRequestData : public IDisplayRequest
    {
    public:

		DisplayRequestData();

        /**
         * DisplayRequestData
         *
         * Standard constructor.
         *
         * @param       
         */

        DisplayRequestData( std::string pkey );


        /**
         * ~TrainTimeScheduleData
         *
         * Standard destructor.
         */

        virtual ~DisplayRequestData();
        
        /**
         * getKey
         *
         * Gets all the presets for this Camera.
         *
         * @return      PresetMap
         *              The map of preset indices to names.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */

        virtual std::string getKey();

		virtual std::string getTimestamp();
		virtual std::string getDestinationSystem();
		virtual std::string getSubmittedBy();
		virtual int getMessageType();
		virtual int getMessageTag();
		virtual int getLibrarySection();
		virtual unsigned short getLibraryVersion();
		virtual std::string getAdhocMessageContent();
		virtual std::string getStartTime();
		virtual std::string getEndTime();
		virtual bool getContinuous();
		virtual int getPriority();
		virtual std::string getRatisTag();
		virtual std::string getRatisDestination();
		virtual bool getRatisOverridable();
		virtual std::string getStisDestinationStation();
		virtual std::string getStisDestinationLevels();
		virtual std::string getStisDestinationPids();
		virtual std::string getTtisDestinationTrainId();
		virtual std::string getTtisDestinationPids();
		virtual TA_Base_Core::DisplayAttributes getDisplayAttributes();
		virtual TA_Base_Core::PlasmaAttributes getPlasmaAttributes();
		virtual TA_Base_Core::LEDAttributes getLEDAttributes();
		virtual time_t getCreatedTime();
		//hongran++ TD17502
		virtual TA_Base_Core::IfSendSuccess getIfSendSuccess();
		//++hongran TD17502


		virtual void setTimestamp( const std::string& timestamp );
		virtual void setDestinationSystem( const std::string& destinationSystem );
		virtual void setSubmittedBy( const std::string& submittedBy );
		virtual void setMessageType( int messageType );
		virtual void setMessageTag( int messageTag );
		virtual void setLibrarySection( int librarySection );
		virtual void setLibraryVersion( unsigned short libraryVersion );
		virtual void setAdhocMessageContent( const std::string& adhocMessageContent );
		virtual void setStartTime( const std::string& startTime );
		virtual void setEndTime( const std::string& endTime );
		virtual void setContinuous( bool continuous );
		virtual void setPriority( int priority );
		virtual void setRatisTag( const std::string& ratisTag );
		virtual void setRatisDestination( const std::string& destination );
		virtual void setRatisOverridable( bool overridable );
		virtual void setStisDestinationStation( const std::string& destinationStation );
		virtual void setStisDestinationLevels( const std::string& destinationLevels );
		virtual void setStisDestinationPids( const std::string& destinationPids );
		virtual void setTtisDestinationTrainId( const std::string& destinationTrainId );
		virtual void setTtisDestinationPids( const std::string& destinationPids );
		virtual void setDisplayAttributes( TA_Base_Core::DisplayAttributes attributes );
		virtual void setPlasmaAttributes( TA_Base_Core::PlasmaAttributes attributes );
		virtual void setLEDAttributes( TA_Base_Core::LEDAttributes attributes );

		//hongran++ TD17502
		virtual void setIfSendSuccess(TA_Base_Core::IfSendSuccess successful);
		//++hongran TD17502

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

        virtual void populateData( boost::shared_ptr< TA_Base_Core::IData > data, unsigned long index );

        /** 
          * timestampToString
          *
          * given a timestamp, convert it to a displayable format.
          *
          * @param timestamp the string in YYYYMMDDHHMMSS convert it to a readable string.
          *
          * @return a readable string.
          */
        static std::string timestampToString(const std::string& timestamp);

        /** 
          * startEndTimeToString
          *
          * Given a start or end time, convert it to a string.
          *
          * @param time the time to convert.
          *        One of two formats. YYYYMMDDHHMMSS (STIS style)
          *        or HHMM (TTIS style)
          *
          * @return a printable string
          */
        static std::string startEndTimeToString(const std::string& time);

        /** 
          * messageTypeToString
          *
          * convert the message type to a displayable string.
          *
          * @param messageType The integer representation of the message type
          * @param librarySection The library section for a predefined message - normal or emergency
          *
          * @return a string representation of the message type
          */
        static std::string messageTypeToString(int messageType, int librarySection);

        /** 
          * priorityToString
          *
          * convert the priority to a displayable string.
          *
          * @param priority The integer representation of the priority
          *
          * @return a string representation of the priority
          */
        static std::string priorityToString(int priority);

        /** 
          * ratisTagToString
          *
          * convert the ratis Tag to a displayable string.
          *
          * @param ratisTag The ratis Tag (may be empty)
          *
          * @return a displayable ratis Tag
          */
        static std::string ratisTagToString(std::string ratisTag);

    private:
        //
        // The copy constructor and assignment operator aren't needed so they are made
        // private here to hide them and prevent their use.
        //
        DisplayRequestData& operator=( const DisplayRequestData& );
        DisplayRequestData( const DisplayRequestData& );

		DisplayRequestHelper* m_displayRequestHelper;

    };

} //end namespace TA_IRS_Core

#endif 

