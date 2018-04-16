/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioSubscriberFactory.h $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * This is the access factory used to load/create radio subscriber data.
  */

#if !defined(RadioSubscriberFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_)
#define RadioSubscriberFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_

#include <string>
#include "bus/radio/radio_bus/src/radiotypes.h"

// forward declarations
namespace TA_IRS_Core
{
    class IRadioSubscriber;
	class IConfigRadioSubscriber;
}


namespace TA_IRS_Core
{
    class RadioSubscriberFactory
    {

    public:


	    /**
	      * ~RadioSubscriberFactory
	      * 
	      * Destructor
	      */
	    virtual ~RadioSubscriberFactory();

	    /**
	      * getInstance
	      * 
	      * Creates and returns an instance of the singleton access factory.
	      * 
	      * @return RadioSubscriberFactory& 
	      */
	    static RadioSubscriberFactory& getInstance();

	    /**
	      * removeInstance
	      * 
	      * Removes the instance of the class (if already created) and cleans up the members.  
          * Primarily used upon program termination (e.g. from main()) so that Purify does not 
          * consider this class and its members to be memory leaks.
	      * 
	      * @return void 
	      */
        static void removeInstance();

        /**
          * getAllRadioSubscribers
          *
          * Retrieves all RadioSubscribers and returns the associated data as a vector of  
          * objects conforming to the RadioSubscriber interface.
          *
          * @return  std::vector<IRadioSubscriber*> 
          *          A vector of pointers to objects conforming to the IRadioSubscriber interface.
          *          Note: It is the responsibility of the caller (client) to delete all
          *          the returned IRadioSubscriber objects.
          * @param   const bool readWrite (optional)
          *             set to true to return write-able ConfigRadioSubscriber objects or 
          *             set to false to return read-only RadioSubscriber objects.          
          *
          * @exception DatabaseException 
          *            - if there are any problems in communicating with the database.
          *            The possible reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException 
          *             - if there is no RadioSubscriber matching the supplied key
          *             - if there is more than one RadioSubscriber matching the supplied key.          
          */
        std::vector<IRadioSubscriber*> getAllRadioSubscribers(const bool readWrite /* = false*/);
        
        /**
          * getRadioSubscriber
          *
          * Retrieves the specified RadioSubscriber and returns the associated data as an 
          * object conforming to the IRadioSubscriber interface.
          *
          * @return  A pointer to an object conforming to the IRadioSubscriber interface.
          *          Note: It is the responsibility of the caller/*client* to delete
          *          the returned IRadioSubscriber object.
          * @param   key        the primary key of the RadioSubscriber to retrieve
          * @param   readWrite  (optional) set to true to return writable
          *                     IConfigRadioSubscriber objects or set to false to return 
          *                     read only IRadioSubscriber objects.
          *
          * @exception  DatabaseException 
          *              - if there are any problems in communicating with the database.
          *              The possible reasons are:
          *              1) Invalid connection string (as stored in RunParams)
          *              2) Database is not/cannot be opened
          *              3) Error while attempting to execute a database query
          * @exception   DataException 
          *              - if there is no RadioSubscriber matching the supplied key
          *              - if there is more than one RadioSubscriber matching the supplied key. 
          */
		IRadioSubscriber* getRadioSubscriber(const unsigned long key, const bool readWrite = false);

        

        /**
        * changeRadioSubscriber
        * 
        * add or update the Radio subscriber with the particular owner, ownerflag and resource
        * 
        * @return void 
        * @param : unsigned long owner
        * @param : char ownerflag
        * @param : RadioResource resource
        * 
        * @exception <exceptions> Optional
        */
        void changeRadioSubscriber(const RadioResource&  resource);

        /**
        * deleteRadioSubscriber
        * 
        * delete the radio subscriber with the particular owner and ownerflag
        * 
        * @return void 
        * @param : unsigned long owner
        * @param : char ownerflag
        * 
        * @exception <exceptions> Optional
        */
        void deleteRadioSubscriber(const RadioResource&  resource);  

        /**
        * getRadioSubscriberBySSI
        * 
        * retrieves the radio subscriber with the particular ssi
        * 
        * @return IRadioSubscriber* 
        * @param : unsigned long ssi
        * 
        * @exception <exceptions> Optional
        */
        IRadioSubscriber* getRadioSubscriberBySSI(unsigned long ssi); 
		
        
        IConfigRadioSubscriber* createNew(const RadioResource&  resource);


		IConfigRadioSubscriber* createNewSubscriber();

		//TD 14848
		//zhou yuan++
		std::vector<IRadioSubscriber*> getRadioSubscribersByResourseType( const char resourseType, const bool readWrite = false );
		//++zhou yuan


		/**
          * getRadioSubscribersByResourceAlias
          *
          * Get corresponding talkgroup from database given a talkgroup alias
          *
          * @return  A pointer to an object conforming to the IRadioSubscriber interface.
          * @param   resourceAlias talkgroup alias
          *
          * @exception  DatabaseException 
          *              - if there are any problems in communicating with the database.
          *              The possible reasons are:
          *              1) Invalid connection string (as stored in RunParams)
          *              2) Database is not/cannot be opened
          *              3) Error while attempting to execute a database query
          */
		std::vector<IRadioSubscriber*> getRadioSubscribersByResourceAlias(const std::string resourseAlias, const bool readWrite);

		IConfigRadioSubscriber* copyRadioSubscriber(const IConfigRadioSubscriber* radioSubscriberToCopy); 

		/** 
		 * updateRadioSubscriber
		 *
		 * Update existing radio subscriber entry given the primary key
		 *
		 * @return void
		 * @param  key  primary key of the resource
		 *		   newResource new details of the radio resource
		 * @exception DatabaseException
         *              - if there are any problems in communicating with the database.
         *              The possible reasons are:
         *              1) Invalid connection string (as stored in RunParams)
         *              2) Database is not/cannot be opened
         *              3) Error while attempting to execute a database query
		 */
		void updateRadioSubscriber(unsigned long key, const RadioResource& newResource);

    private:

	    /**
	      * RadioSubscriberFactory
	      * 
	      * Default constructor (privatized)
	      */
        RadioSubscriberFactory();

	    /**
	      * RadioSubscriberFactory
	      * 
	      * Copy constructor (disabled)
	      * 
	      * @param : const RadioSubscriberFactory& theRadioSubscriberFactory
	      */
		RadioSubscriberFactory(const RadioSubscriberFactory&);

	    /**
	      * operator=
	      * 
	      * Assignment operator (disabled)
	      * 
	      * @return RadioSubscriberFactory& 
	      * @param : const RadioSubscriberFactory&
	      */
		RadioSubscriberFactory& operator=(const RadioSubscriberFactory&);

    private:
        
	    static RadioSubscriberFactory* m_instance;
        


    };
}
#endif // !defined(RadioSubscriberFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_)
