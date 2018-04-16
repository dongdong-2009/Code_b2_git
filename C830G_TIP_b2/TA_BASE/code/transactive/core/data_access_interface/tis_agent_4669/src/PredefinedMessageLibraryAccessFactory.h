#ifndef PREDEFINED_MESSAGE_LIBRARY_ACCESS_FACTORY_H_INCLUDED
#define PREDEFINED_MESSAGE_LIBRARY_ACCESS_FACTORY_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * PredefinedMessageLibraryAccessFactory is a singleton that is used to generate and retrieve 
  * IPredefinedMessageLibrary objects.
  */

#include <string>
#include <vector>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/tis_agent_4669/src/IConfigPredefinedMessageLibrary.h"

namespace TA_Base_Core
{
   class IPredefinedMessageLibrary;    // Forward declaration.
}

namespace TA_Base_Core
{
    
    class PredefinedMessageLibraryAccessFactory
    {
    public:

        /**
         * ~TrainTimeScheduleAccessFactory
         *
         * Standard destructor.
         */

        virtual ~PredefinedMessageLibraryAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      TrainTimeScheduleAccessFactory&
         *              A reference to an instance of a TrainTimeScheduleAccessFactory object.
         */

		static PredefinedMessageLibraryAccessFactory& getInstance();


        /**
         * getPredefinedMessageLibraries
         *
         * Retrieves all messages libraries in the ti_predefined_message_library table
         *
         * @return      std::vector<IPredefinedMessageLibrary*>
         *              A vector of pointers to objects conforming to the IPredefinedMessageLibrary interface.
         *              NOTE: The caller is responsible for cleaning up this pointer.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */

		std::vector<IPredefinedMessageLibrary*> getPredefinedMessageLibraries();

        /**
         * getPredefinedMessageLibrary
         *
         * Retrieves the messages librariy in the ti_predefined_message_library table
         * with the specified version and type
         *
         * @return      IPredefinedMessageLibrary*
         *              A pointer to an object conforming to the IPredefinedMessageLibrary interface.
         *              NOTE: The caller is responsible for cleaning up this pointer.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */
		IPredefinedMessageLibrary* getPredefinedMessageLibrary( unsigned short version, const std::string& libraryType );

		//hongran++ TD17500
		// Used to get predefined msg from OCC local DB
		IPredefinedMessageLibrary* getOCCLocalPredefinedMessageLibrary( unsigned short version, const std::string& libraryType );

		// Used to delete data from OCC local DB
		void deleteOCCLocalAllMessageLibrariesOfTypeNotMatching( const std::string& libraryType, unsigned short currentVersion, unsigned short nextVersion );
		//++hongran TD17500


		IConfigPredefinedMessageLibrary* createNewPredefinedMessageLibrary( unsigned short version, const std::string& libraryType,
			std::vector<unsigned char>& libraryFileContent );

        /**
         * deleteAllMessageLibrariesOfTypeNotMatching
         *
         * Delete all messages libraries in ti_predefined_message_library table of type libraryType
         * not matching specified version
         *
         * @return      nothing
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         */
		void deleteAllMessageLibrariesOfTypeNotMatching( const std::string& libraryType, unsigned short currentVersion, unsigned short nextVersion );

		PredefinedMessage getPredefinedMessage( unsigned short version, std::string& libraryType, 
												int librarySection, unsigned short messageTag);

    private:
        //
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        //
        PredefinedMessageLibraryAccessFactory() {};
        PredefinedMessageLibraryAccessFactory& operator=(const PredefinedMessageLibraryAccessFactory &) {};
        PredefinedMessageLibraryAccessFactory( const PredefinedMessageLibraryAccessFactory& ) {};


        /** 
          * getPredefinedMessageLibrariesWhere
          *
          * gets the pre-defined message libraries based on some criteria
          *
          * @param whereSQL an SQL where clause.
          *
          * @return the pre-defined message libraries retrieved.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
        std::vector<IPredefinedMessageLibrary*> getPredefinedMessageLibrariesWhere(std::string whereSQL = "");

		std::vector<IPredefinedMessageLibrary*> getOCCLocalPredefinedMessageLibrariesWhere(std::string whereSQL = "");


		static PredefinedMessageLibraryAccessFactory* m_theClassInstance;

		//
        // Thread lock to protect singleton creation.
        //
        
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;
    };

} // end namespace TA_Base_Core

#endif
