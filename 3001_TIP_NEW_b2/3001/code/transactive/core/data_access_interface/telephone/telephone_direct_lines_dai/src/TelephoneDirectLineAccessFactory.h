/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/telephone/telephone_direct_lines_dai/src/TelephoneDirectLineAccessFactory.h $
  * @author Raymond Pau
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * TelephoneDirectLineAccessFactory is a singleton that is used to retrieve TelephoneDirectLine objects either from the
  * database or newly created. All TelephoneDirectLine objects returned will adhere to the ITelephoneDirectLine interface.
  */

#if !defined(TelephoneDirectLineAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define TelephoneDirectLineAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <list>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/ITelephoneDirectLine.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/IConfigTelephoneDirectLine.h"

namespace TA_IRS_Core
{


    class TelephoneDirectLineAccessFactory
    {


    private:
        TelephoneDirectLineAccessFactory(){};

        // Disable the following methods
	    TelephoneDirectLineAccessFactory(const TelephoneDirectLineAccessFactory& theTelephoneDirectLineAccessFactory);
	    TelephoneDirectLineAccessFactory& operator=(const TelephoneDirectLineAccessFactory &);

    public:
        /** 
         * ~TelephoneDirectLineAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~TelephoneDirectLineAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an TelephoneDirectLineAccessFactory object.
         */
	    static TelephoneDirectLineAccessFactory& getInstance();

        /**
         * removeInstance
	     *
	     * Removes the instance of the class (if already created) and cleans up the members.  
         * Primarily used upon program termination (e.g. from main()) so that Purify does not 
         * consider this class and its members to be memory leaks.
         *
         */
        static void removeInstance();

        /**
	      * getButtonDetails
	      * 
	      * Call this function to retrieve the details of a particular button that is associated with 
          * this setName.  User of this function have to delete the returned pointer after usage.
	      * 
	      * @return ITelephoneDirectLine* 
	      * @param : const unsigned long operatorKey
	      * @param : const unsigned long profileId
	      * @param : const std::string setName
	      * @param : const int btnNum
	      * @param : bool readWrite = false
	      * 
	      * @exception DatabaseException This exception will be thrown under the followin circumstances:
          *            1) The database has inadvertantly been closed, and cannot be reopend.
          *            2) The exectuion of the sql query fails.
	      */
        ITelephoneDirectLine* getButtonDetails( const long operatorKey, 
                                                const long profileId, 
                                                const std::string setName, 
                                                const int btnNum, 
                                                bool readWrite = false);

	    /**
	      * getButtonSet
	      * 
	      * Call this function to retrieve all the buttons that are associated with 
          * this setName.  User of this function have to delete the returned list pointer 
          * as well as contents in the list itself after its usage.
	      * 
	      * @return ITelephoneDirectLines* 
	      * @param : const unsigned long operatorKey
	      * @param : const unsigned long profileId
	      * @param : const std::string setName
	      * @param : bool readWrite = false
	      * 
	      * @exception DatabaseException This exception will be thrown under the followin circumstances:
          *            1) The database has inadvertantly been closed, and cannot be reopend.
          *            2) The exectuion of the sql query fails.
	      */
        ITelephoneDirectLines getButtonSet( const long operatorKey, 
                                            const long profileId, 
                                            const std::string setName, 
                                            bool readWrite = false);

	    /**
	      * getSetDetails
	      * 
	      * Check if there are sets already defined in the database for this Operator Key and profile ID
	      * 
	      * @return bool 
	      * @param : const unsigned long operatorKey
	      * @param : const unsigned long profileId
	      * @param : const unsigned long setName
	      * @param : const unsigned long readWrite
	      * 
	      * @exception DatabaseException This exception will be thrown under the followin circumstances:
          *            1) The database has inadvertantly been closed, and cannot be reopend.
          *            2) The exectuion of the sql query fails.
	      */
        ITelephoneDirectLine* getSetDetails( const long operatorKey, 
                                             const long profileId, 
                                             const std::string setName, 
                                             bool readWrite = false);



	    /**
	     * createNewButtonSetRecord
         * 
         * Creates an empty IConfigTelephoneDirectLine object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigTelephoneDirectory object.
         *
         * @return A pointer to an IConfigTelephoneDirectory object
	     * 
	     * @return ITelephoneDirectLine* 
	     * @param : bool readWrite = false
	     * 
	     * @exception <exceptions> Optional
	     */
        ITelephoneDirectLine* createNewButtonSetRecord(bool readWrite = false);

    private:

        static TelephoneDirectLineAccessFactory* m_instance;

    };
} // close TA_IRS_Core
#endif // !defined(TelephoneDirectLineAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
