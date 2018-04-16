/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioTextMessageInboxFactory.h $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $ Haipeng
  * 1: changed the const variables difinition
  * This is the access factory used to load/create radio textmessage data.
  */

#if !defined(RadioTextMessageInboxFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_)
#define RadioTextMessageInboxFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_

#include <string>

// forward declarations
namespace TA_IRS_Core
{
    class IRadioTextMessageInbox;
}


namespace TA_IRS_Core
{
    class RadioTextMessageInboxFactory
    {

    public:


	    /**
	      * ~RadioTextMessageInboxFactory
	      * 
	      * Destructor
	      */
	    virtual ~RadioTextMessageInboxFactory();

	    /**
	      * getInstance
	      * 
	      * Creates and returns an instance of the singleton access factory.
	      * 
	      * @return RadioTextMessageInboxFactory& 
	      */
	    static RadioTextMessageInboxFactory& getInstance();

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
          * getAllRadioTextMessageInbox
          *
          * Retrieves all RadioTextMessageInboxs and returns the associated data as a vector of  
          * objects conforming to the RadioTextMessageInbox interface.
          *
          * @return  std::vector<IRadioTextMessageInbox*> 
          *          A vector of pointers to objects conforming to the IRadioTextMessageInbox interface.
          *          Note: It is the responsibility of the caller (client) to delete all
          *          the returned IRadioTextMessageInbox objects.
          * @param   const bool readWrite (optional)
          *             set to true to return write-able ConfigRadioTextMessageInbox objects or 
          *             set to false to return read-only RadioTextMessageInbox objects.          
          *
          * @exception DatabaseException 
          *            - if there are any problems in communicating with the database.
          *            The possible reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException 
          *             - if there is no RadioTextMessageInbox matching the supplied key
          *             - if there is more than one RadioTextMessageInbox matching the supplied key.          
          */
        std::vector<IRadioTextMessageInbox*> getAllRadioTextMessageInboxs(const std::string& sessionId, const bool readWrite = false);
        
        /**
          * getRadioTextMessageInbox
          *
          * Retrieves the specified RadioTextMessageInbox and returns the associated data as an 
          * object conforming to the IRadioTextMessageInbox interface.
          *
          * @return  A pointer to an object conforming to the IRadioTextMessageInbox interface.
          *          Note: It is the responsibility of the caller/*client* to delete
          *          the returned IRadioTextMessageInbox object.
          * @param   key        the primary key of the RadioTextMessageInbox to retrieve
          * @param   readWrite  (optional) set to true to return writable
          *                     IConfigRadioTextMessageInbox objects or set to false to return 
          *                     read only IRadioTextMessageInbox objects.
          *
          * @exception  DatabaseException 
          *              - if there are any problems in communicating with the database.
          *              The possible reasons are:
          *              1) Invalid connection string (as stored in RunParams)
          *              2) Database is not/cannot be opened
          *              3) Error while attempting to execute a database query
          * @exception   DataException 
          *              - if there is no RadioTextMessageInbox matching the supplied key
          *              - if there is more than one RadioTextMessageInbox matching the supplied key. 
          */
		IRadioTextMessageInbox* getRadioTextMessageInbox(const unsigned long key, const bool readWrite = false);

        /**
          * createRadioTextMessageInbox
          * 
          * Creates an empty ConfigRadioTextMessageInbox object, and returns a pointer to it.
          * It is the responsibility of the caller (client) to delete the returned  
          * ConfigRadioTextMessageInbox object.
          *
          * @return IConfigRadioTextMessageInbox* - A pointer to an IConfigRadioTextMessageInbox object
          */
	  //  IConfigRadioTextMessageInbox* createRadioTextMessageInbox();

        IRadioTextMessageInbox* addNew(
			time_t timestamp,const std::string& sessionId,char isread,
			const std::string& msgtext,const std::string& source, const std::string& recipient);

        // set the read flag
    	void setRead(const unsigned long key , bool flag);

    private:

	    /**
	      * RadioTextMessageInboxFactory
	      * 
	      * Default constructor (privatized)
	      */
        RadioTextMessageInboxFactory();

	    /**
	      * RadioTextMessageInboxFactory
	      * 
	      * Copy constructor (disabled)
	      * 
	      * @param : const RadioTextMessageInboxFactory& theRadioTextMessageInboxFactory
	      */
		RadioTextMessageInboxFactory(const RadioTextMessageInboxFactory&);

	    /**
	      * operator=
	      * 
	      * Assignment operator (disabled)
	      * 
	      * @return RadioTextMessageInboxFactory& 
	      * @param : const RadioTextMessageInboxFactory&
	      */
		RadioTextMessageInboxFactory& operator=(const RadioTextMessageInboxFactory&);
		
        /**
        * Helper method to get operator key for this session
        */
        unsigned long getOperatorKey(const std::string& sessionId);
		

    private:
        
	    static RadioTextMessageInboxFactory* m_instance;

    };
}
#endif // !defined(RadioTextMessageInboxFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_)
