#ifndef CONFIGPREDEFINED_MESSAGE_LIBRARY_DATA_H_INCLUDED
#define CONFIGPREDEFINED_MESSAGE_LIBRARY_DATA_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/tis_agent_4669/src/ConfigPredefinedMessageLibraryData.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class contains the database retrievals and writes for Train Time Schedules
  * and their associated schedule entries, for the Traveller Information System (TIS) Agent.
  * It implements the ITrainTimeSchedule interface.
  */

#include <map>
#include "core/data_access_interface/tis_agent_4669/src/IConfigPredefinedMessageLibrary.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryHelper.h"

namespace TA_Base_Core
{
    class ConfigPredefinedMessageLibraryData : public IConfigPredefinedMessageLibrary
    {
    public:

        /**
         * ConfigPredefinedMessageLibraryData
         *
         * Standard constructor.
         *
         */
        ConfigPredefinedMessageLibraryData( );



        /** 
          * ConfigPredefinedMessageLibraryData
          *
          * Constructor with initial values
          *
          * @param version  The library version
          * @param libraryType  The library type (STIS or TTIS)
          * @param libraryFileContent   The raw library file
          *
          */
        ConfigPredefinedMessageLibraryData( unsigned short version,
                                            const std::string& libraryType,
                                            std::vector<unsigned char>& libraryFileContent );

        /**
         * ~ConfigPredefinedMessageLibraryData
         *
         * Standard destructor.
         */
        virtual ~ConfigPredefinedMessageLibraryData();
        

        /**
         * getKey
         *
         * @return      The primary key of this item in the database.
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
        virtual unsigned long getKey();


        /** 
          * getVersion
          *
          * Retrieves the pre-defined message libraries version
          *
          * @return The version of the pre-defined message library.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
		virtual unsigned short getVersion();


        /** 
          * getLibraryType
          *
          * Gets the type of library, TTIS or STIS.
          *
          * @return the string TTIS or the string STIS.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
		virtual std::string getLibraryType();


        /** 
          * getLibraryFileContent
          *
          * Get the raw content of the library file.
          *
          * @param fileContent  The vector of characters to copy the library file into.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
		virtual void getLibraryFileContent( std::vector<unsigned char>& fileContent );


        /**
         * getMessages
         *
         * Retrieves all entries (stored in the ti_train_time_schedule_entry table)
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
         *              Thrown if the data cannot be converted to the correct format.
         */
		virtual PredefinedMessageList getMessages();


        /** 
          * getMessage
          *
          * get the message for the given message section and tag.
          *
          * @param librarySection
          * @param messageTag
          *
          * @return a pointer to the matching message. NULL if not found.
          *         note that this class retains ownership of the data returned.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
        virtual PredefinedMessage* getMessage(int librarySection, unsigned short messageTag);


        /** 
          * getDefaultSTISLedAttributes
          *
          * Get the default STIS led attributes associated with
          * this message library.
          *
          * @return the structure holding the attribute values.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
        virtual STISLedAttributes getDefaultSTISLedAttributes();


        /** 
          * getDefaultSTISPlasmaAttributes
          *
          * Get the default STIS plasma attributes associated with
          * this message library.
          *
          * @return the structure holding the attribute values.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
        virtual STISPlasmaAttributes getDefaultSTISPlasmaAttributes();


        /** 
          * getDefaultTTISLedAttributes
          *
          * Get the default TTIS led attributes associated with
          * this message library.
          *
          * @return the structure holding the attribute values.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
        virtual TTISLedAttributes getDefaultTTISLedAttributes();


        /**
          * addMessage
          *
          * Adds a pre-defined message to this library.
          *
          * @param       theMessage
          *              The message to add to this library
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
         virtual void addMessage( const PredefinedMessage& theMessage );


         /** 
          * setDefaultDisplayAttributes
          *
          * Set the default STIS led, STIS plasma, and TTIS led attributes associated with
          * this message library.
          *
          * @param stisLedAttributes the structure holding the stis led attribute values.
          * @param stisPlasmaAttributes the structure holding stis plasma the attribute values.
          * @param ttisLedAttributes the structure holding the ttis led attribute values.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
        virtual void setDefaultDisplayAttributes( const STISLedAttributes& stisLedAttributes,
                                                  const STISPlasmaAttributes& stisPlasmaAttributes,
                                                  const TTISLedAttributes& ttisLedAttributes );


         /** 
          * deleteThisLibrary
          *
          * Deletes the pre-defined message library and all associated data
          * from the database.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
         virtual void deleteThisLibrary();


         /**
		  * invalidate
		  *
		  * Marks the data contained by this object as invalid. The next call to get...() 
		  * following a call to invalidate() will cause all the data to be reloaded from
		  * the database.
		  */
		virtual void invalidate();

        
		// IConfigItem methods
		virtual unsigned long getUniqueIdentifier() { return m_uniqueIdentifier; };

		virtual TA_Base_Core::ItemChanges getAllItemChanges() { return m_messageLibraryDataChanges; };

		virtual bool hasChanged() { return !m_messageLibraryDataChanges.empty(); };

		virtual bool isNew();

		void applyChanges();


    private:

		// These methods do not apply to our table so we make it private and inaccessible
		virtual void setName(const std::string &name) { };
		virtual std::string getName() { std::string blank(""); return blank; };


        //
        // The copy constructor and assignment operator aren't needed so they are made
        // private here to hide them and prevent their use.
        //
        ConfigPredefinedMessageLibraryData& operator=( const ConfigPredefinedMessageLibraryData& );
        ConfigPredefinedMessageLibraryData( const ConfigPredefinedMessageLibraryData& );

        //
        // Holds all the messages contained in this predefined message library
        //
        IPredefinedMessageLibrary::PredefinedMessageList m_messages;

		PredefinedMessageLibraryHelper* m_messageLibraryHelper;

        //
        // This indicates whether we currently have valid data or not. This is
        // initialised to false and is set to true once we do a read from the
        // database.
        //
        bool m_isValidData;

		TA_Base_Core::ItemChanges	m_messageLibraryDataChanges;

		unsigned long				m_uniqueIdentifier;
    };

} //end namespace TA_Base_Core

#endif 
