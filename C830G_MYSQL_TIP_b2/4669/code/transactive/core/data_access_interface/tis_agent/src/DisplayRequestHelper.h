#ifndef DISPLAY_REQUEST_HELPER_H_INCLUDED
#define DISPLAY_REQUEST_HELPER_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/DisplayRequestHelper.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * DisplayRequestHelper contains the get and set methods that do the 
  * actual database work for the DisplayRequestData class.
  * It accesses the ti_display_request table.
  */

#pragma warning ( disable : 4786 )

#include <map>
#include <climits>
#include <boost/shared_ptr.hpp>

#include "core/data_access_interface/tis_agent/src/IDisplayRequest.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


namespace TA_Base_Core
{
    class IData;
}


namespace TA_IRS_Core
{
    class DisplayRequestHelper
    {
    public:
        
		DisplayRequestHelper();

       /**
         * DisplayRequestHelper
         * 
         * This constructor creates a new DisplayRequestHelper for the specified key.
         *
         * @param       unsigned long displayRequestPKey
         *              The database key for the DisplayRequest parent of this helper.
         */

        DisplayRequestHelper( std::string displayRequestPKey );

        /**
          * DisplayRequestHelper
          *
          * Standard copy constructor.
          */

        DisplayRequestHelper( const DisplayRequestHelper& theDisplayRequestHelper );

        /**
         * ~DisplayRequestHelper
         *
         * Standard destructor.
         */

        virtual ~DisplayRequestHelper() {};

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

		std::string getKey();
		std::string getTimestamp();
		std::string getDestinationSystem();
		std::string getSubmittedBy();
		int getMessageType();
		int getMessageTag();
		int getLibrarySection();
		unsigned short getLibraryVersion();
		std::string getAdhocMessageContent();
		std::string getStartTime();
		std::string getEndTime();
		bool getContinuous();
		int getPriority();
		std::string getRatisTag();
		std::string getRatisDestination();
		bool getRatisOverridable();
		std::string getStisDestinationStation();
		std::string getStisDestinationLevels();
		std::string getStisDestinationPids();
		std::string getTtisDestinationTrainId();
		std::string getTtisDestinationPids();
        TA_Base_Core::DisplayAttributes getDisplayAttributes();
		TA_Base_Core::PlasmaAttributes getPlasmaAttributes();
		TA_Base_Core::LEDAttributes getLEDAttributes();
		time_t getCreatedTime();
		//hongran++ TD17502 
		TA_Base_Core::IfSendSuccess getIfSendSuccess();
		//++hongran TD17502

		void setTimestamp( const std::string& timestamp );
		void setDestinationSystem( const std::string& destinationSystem );
		void setSubmittedBy( const std::string& submittedBy );
		void setMessageType( int messageType );
		void setMessageTag( int messageTag );
		void setLibrarySection( int librarySection );
		void setLibraryVersion( unsigned short libraryVersion );
		void setAdhocMessageContent( const std::string& adhocMessageContent );
		void setStartTime( const std::string startTime );
		void setEndTime( const std::string endTime );
		void setContinuous( bool continuous );
		void setPriority( int priority );
		void setRatisTag( const std::string& ratisTag );
		void setRatisDestination( const std::string& destination );
		void setRatisOverridable( bool overridable );
		void setStisDestinationStation( const std::string& destinationStation );
		void setStisDestinationLevels( const std::string& destinationLevels );
		void setStisDestinationPids( const std::string& destinationPids );
		void setTtisDestinationTrainId( const std::string& destinationTrainId );
		void setTtisDestinationPids( const std::string& destinationPids );
		void setDisplayAttributes( TA_Base_Core::DisplayAttributes attributes );
		void setPlasmaAttributes( TA_Base_Core::PlasmaAttributes attributes );
		void setLEDAttributes( TA_Base_Core::LEDAttributes attributes );

		//hongran++ TD17502
		void setIfSendSuccess(TA_Base_Core::IfSendSuccess successful);
		//++hongran TD17502

		bool isNew() const { return m_isNew; };

		bool hasChanged() {return m_hasChanged;}

 		void addNewDisplayRequestData();

		void writeDisplayRequestData();

		void updateExistingDisplayRequestData();

		void deleteThisDisplayRequest();

        /**
         * invalidate
         *
         * Marks the data contained by this VideoOutputGroup as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    Either - this VideoOutputGroup was initially loaded from the database
         *                  OR     - writeVideoOutputGroupData() has already been called, as it does 
         *                           not make any sense to invalidate a VideoOutputGroup that has not 
         *                           yet been written to the database.
         */
	    void invalidate();


        //limin++
        void populateData( boost::shared_ptr< TA_Base_Core::IData > data, unsigned long index );
        static const std::vector< std::string >& getColumnNames();
        static const std::string& getColumnNamesSql();
        //++limin

    private:
        //
        // Disable the assignment operator.
        //

		DisplayRequestHelper& operator=( const DisplayRequestHelper& );

		std::string getLocalDatabaseName();

        
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

		//hongran++ TD17502
		std::string convertSendSuccessToStr(TA_Base_Core::IfSendSuccess success);
		//++hongran TD17502

        //++limin
        long getIntegerDataDefault( const std::string& column, long def );
        //++limin

         //
        // Flag to indicate if the data needs reloading.
        //

	    bool m_isValidData;

        //
        // Flag to indicate if this is a new VideoOutputGroup or an existing one
        // loaded from the database.
        //

        bool m_isNew;

		bool m_hasChanged;
        
        //
        // The DisplayRequest database primary key 
        //
		std::string m_key;

		std::string m_timestamp;
		std::string m_destinationSystem;
		std::string m_submittedBy;
		int m_messageType;
		int m_messageTag;
		int m_librarySection;
		unsigned short m_libraryVersion;
		std::string m_adhocMessageContent;
		std::string m_startTime;
		std::string m_endTime;
		bool m_continuous;
		int m_priority;
		std::string m_ratisTag;
		std::string m_ratisDestination;
		bool m_ratisOverridable;

		std::string m_stisDestinationStation;
		std::string m_stisDestinationLevels;
		std::string m_stisDestinationPids;
		std::string m_ttisDestinationTrainId;
		std::string m_ttisDestinationPids;

		TA_Base_Core::DisplayAttributes m_displayAttributes;
		TA_Base_Core::PlasmaAttributes m_plasmaAttributes;
		TA_Base_Core::LEDAttributes m_ledAttributes;

		time_t m_createTime;

		std::string m_localDatabase;

		//hongran++ TD17502
		TA_Base_Core::IfSendSuccess m_ifSendSuccess;
		//++hongran TD17502

        static TA_Base_Core::NonReEntrantThreadLockable s_columnNamesLock;
        static std::vector< std::string > s_columnNames;

        static std::string s_columnNamesSql;
        static TA_Base_Core::NonReEntrantThreadLockable s_columnNamesSqlLock;
    };

} // closes TA_Base_Core

#endif 
