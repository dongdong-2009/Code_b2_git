/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioBtnCfgFactory.h $
  * @author:  Mark See
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#if !defined(RadioBtnCfgFactory_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
#define RadioBtnCfgFactory_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_

#include "bus/radio/radio_bus/src/radiotypes.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"


// forward declarations
namespace TA_IRS_Core
{
    class IRadioBtnCfg;
}

namespace TA_IRS_Core
{
    class RadioBtnCfgFactory
    {
    public:

	    /**
	      * ~RadioBtnCfgFactory
	      * 
	      * Destructor
          * Dummy implementation
          *
	      */
        virtual ~RadioBtnCfgFactory();

	    /**
	      * getInstance
	      * 
	      * Creates and returns an instance of the singleton access factory.
	      * 
	      * @return RadioBtnCfgFactory& 
	      */        
        static RadioBtnCfgFactory& getInstance();

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
          * getAllRadioBtnCfg
          *
          * Retrieves all RadioBtnCfg and returns the associated data as a vector of  
          * objects conforming to the RadioBtnCfg interface.
          *
          * @return  std::vector<IRadioBtnCfg*> 
          *          A vector of pointers to objects conforming to the IRadioBtnCfg interface.
          *          Note: It is the responsibility of the caller (client) to delete all
          *          the returned IRadioBtnCfg objects.
          *
          * @exception DatabaseException 
          *            - if there are any problems in communicating with the database.
          *            The possible reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException 
          *             - if there is no RadioBtnCfg matching the supplied key
          *             - if there is more than one RadioBtnCfg matching the supplied key.          
          */
        std::vector<IRadioBtnCfg*> getAllRadioBtnCfg(const std::string& sessionId);

		/**
		  * getAllRadioBtnCfgByProfile
		  * 
		  * Retrieves all RadioBtnCfg based on Profile 
		  *
          * @return  std::vector<IRadioBtnCfg*> 
          *          A vector of pointers to objects conforming to the IRadioBtnCfg interface.
          *          Note: It is the responsibility of the caller (client) to delete all
          *          the returned IRadioBtnCfg objects.
		  *
          * @exception DatabaseException 
          *            - if there are any problems in communicating with the database.
          *            The possible reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException 
          *             - if there is no RadioBtnCfg matching the supplied key
          *             - if there is more than one RadioBtnCfg matching the supplied key.  
		  */
		std::vector<IRadioBtnCfg*> getAllRadioBtnCfgByProfile(unsigned long profileKey);

        /**
          * getRadioBtnCfg
          *
          * Retrieves the specified RadioBtnCfg and returns the associated data as an 
          * object conforming to the IRadioBtnCfg interface.
          *
          * @return  A pointer to an object conforming to the IRadioBtnCfg interface.
          *          Note: It is the responsibility of the caller/*client* to delete
          *          the returned IRadioBtnCfg object.
          * @param   key        the primary key of the RadioBtnCfg to retrieve
          * @param   readWrite  (optional) set to true to return writable
          *                     IConfigRadioBtnCfg objects or set to false to return 
          *                     read only IRadioBtnCfg objects.
          *
          * @exception  DatabaseException 
          *              - if there are any problems in communicating with the database.
          *              The possible reasons are:
          *              1) Invalid connection string (as stored in RunParams)
          *              2) Database is not/cannot be opened
          *              3) Error while attempting to execute a database query
          * @exception   DataException 
          *              - if there is no RadioBtnCfg matching the supplied key
          *              - if there is more than one RadioBtnCfg matching the supplied key. 
          */

        IRadioBtnCfg* getRadioBtnCfg(unsigned long key);
        
        /**
        * associateButtonWithResource
        * 
        * associate the particular button(btnref) with the radio resource
        */
        void associateButtonWithResource(const std::string& sessionId, bool isProfile, unsigned long btnref, const RadioResource& resource);

        /**
        * clearButtonRef
        * 
        * clear the button with a particular btnref for a particular profile
        * 
        */
        void clearButtonRef(const std::string& sessionId, bool isProfile, unsigned long btnref);

        /**
          * insertPredefinedTG
          *
          * Insert new monitored talkgroups
          *
          * @return  void
          * @param   profileKey profile to be updated
          * @param   btnRef     button reference
		  * @param   resource   talkgroup resource
          *
          * @exception  DatabaseException 
          *              - if there are any problems in communicating with the database.
          *              The possible reasons are:
          *              1) Invalid connection string (as stored in RunParams)
          *              2) Database is not/cannot be opened
          *              3) Error while attempting to execute a database query
          */
		void insertPredefinedTG(unsigned long profileKey, unsigned long btnRef, const RadioResource& resource);
        
		/**
          * updatePredefinedTG
          *
          * Update existing monitored talkgroups for this user
          *
          * @return  void
          * @param   profileKey profile to be updated
          * @param   btnRef     button reference
		  * @param   resource   talkgroup resource
          *
          * @exception  DatabaseException 
          *              - if there are any problems in communicating with the database.
          *              The possible reasons are:
          *              1) Invalid connection string (as stored in RunParams)
          *              2) Database is not/cannot be opened
          *              3) Error while attempting to execute a database query
          */
		void updatePredefinedTG(unsigned long profileKey, unsigned long btnRef, const RadioResource& resource);

		/**
          * deletePredefinedTG
          *
          * Delete monitored talkgroups from this user
          *
          * @return  void
          * @param   profileKey   profile to be updated
          *
          * @exception  DatabaseException 
          *              - if there are any problems in communicating with the database.
          *              The possible reasons are:
          *              1) Invalid connection string (as stored in RunParams)
          *              2) Database is not/cannot be opened
          *              3) Error while attempting to execute a database query
          */
		void deletePredefinedTG(unsigned long profileKey);

    private:
        //Disabled Method
        RadioBtnCfgFactory();
        RadioBtnCfgFactory(const RadioBtnCfgFactory&);
        RadioBtnCfgFactory& operator=(const RadioBtnCfgFactory&);

		/**
		  * getLocalDatabaseName
		  *
		  * Returns the database server name that it will connect to
		  *
		  * @return std::string
		  * 
		  * @exception DatabaseException
		  *            - Unable to find the connection string file
		  *            - Unable to find the data type
		  */
		static std::string getLocalDatabaseName();

    private:
        static RadioBtnCfgFactory* m_instance;
    };
}
#endif // !defined(RadioBtnCfgFactory_3F6F77F3_CAF6_4550_97DE_FE05278F46D9__INCLUDED_)
