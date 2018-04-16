/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioPredefinedTextMessageFactory.h $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $ Haipeng
  * 1: changed the const variables difinition
  * This is the access factory used to load/create radio textmessage data.
  */

#if !defined(RadioPredefinedTextMessageFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_)
#define RadioPredefinedTextMessageFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_

#include <string>

// forward declarations
namespace TA_IRS_Core
{
    class IRadioPredefinedTextMessage;
}


namespace TA_IRS_Core
{
    class RadioPredefinedTextMessageFactory
    {

    public:


	    /**
	      * ~RadioPredefinedTextMessageFactory
	      * 
	      * Destructor
	      */
	    virtual ~RadioPredefinedTextMessageFactory();

	    /**
	      * getInstance
	      * 
	      * Creates and returns an instance of the singleton access factory.
	      * 
	      * @return RadioPredefinedTextMessageFactory& 
	      */
	    static RadioPredefinedTextMessageFactory& getInstance();

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
          * getAllRadioPredefinedTextMessages
          *
          * Retrieves all RadioPredefinedTextMessages and returns the associated data as a vector of  
          * objects conforming to the RadioPredefinedTextMessage interface.
          *
          * @return  std::vector<IRadioPredefinedTextMessage*> 
          *          A vector of pointers to objects conforming to the IRadioPredefinedTextMessage interface.
          *          Note: It is the responsibility of the caller (client) to delete all
          *          the returned IRadioPredefinedTextMessage objects.
          * @param   const bool readWrite (optional)
          *             set to true to return write-able ConfigRadioPredefinedTextMessage objects or 
          *             set to false to return read-only RadioPredefinedTextMessage objects.          
          *
          * @exception DatabaseException 
          *            - if there are any problems in communicating with the database.
          *            The possible reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException 
          *             - if there is no RadioPredefinedTextMessage matching the supplied key
          *             - if there is more than one RadioPredefinedTextMessage matching the supplied key.          
          */
        std::vector<IRadioPredefinedTextMessage*> getAllRadioPredefinedTextMessages(const std::string& sessionId, const bool readWrite/* = false*/);
        
        /**
          * getRadioPredefinedTextMessage
          *
          * Retrieves the specified RadioPredefinedTextMessage and returns the associated data as an 
          * object conforming to the IRadioPredefinedTextMessage interface.
          *
          * @return  A pointer to an object conforming to the IRadioPredefinedTextMessage interface.
          *          Note: It is the responsibility of the caller/*client* to delete
          *          the returned IRadioPredefinedTextMessage object.
          * @param   key        the primary key of the RadioPredefinedTextMessage to retrieve
          * @param   bStatus    True: for the radio status message; otherwise for the userdefined message
		  * @param   readWrite  (optional) set to true to return writable
          *                     IConfigRadioPredefinedTextMessage objects or set to false to return 
          *                     read only IRadioPredefinedTextMessage objects.
          *
          * @exception  DatabaseException 
          *              - if there are any problems in communicating with the database.
          *              The possible reasons are:
          *              1) Invalid connection string (as stored in RunParams)
          *              2) Database is not/cannot be opened
          *              3) Error while attempting to execute a database query
          * @exception   DataException 
          *              - if there is no RadioPredefinedTextMessage matching the supplied key
          *              - if there is more than one RadioPredefinedTextMessage matching the supplied key. 
          */
		IRadioPredefinedTextMessage* getRadioPredefinedTextMessage(const unsigned long key, const bool bStatus, const bool readWrite /* = false*/);

        /**
        * changedPredefinedTextMessage
        * 
        * add or update predefined text message
        * 
        */
        void changedPredefinedTextMessage(const std::string& sessionId, const unsigned long messageKey, const std::string messageName, const std::string messageText);


        /**
        * deletePredefinedTextMessage
        * 
        * delete the predefined text message
        * 
        */
        void deletePredefinedTextMessage(const std::string& sessionId,const unsigned long messageKey);

        /**
          * createRadioPredefinedTextMessage
          * 
          * Creates an empty ConfigRadioPredefinedTextMessage object, and returns a pointer to it.
          * It is the responsibility of the caller (client) to delete the returned  
          * ConfigRadioPredefinedTextMessage object.
          *
          * @return IConfigRadioPredefinedTextMessage* - A pointer to an IConfigRadioPredefinedTextMessage object
          */
	  //  IConfigRadioPredefinedTextMessage* createRadioPredefinedTextMessage();

        IRadioPredefinedTextMessage* createNew(const std::string& sessionId, const std::string messageName, const std::string messageText);

    private:

        // disabled methods
        RadioPredefinedTextMessageFactory();
		RadioPredefinedTextMessageFactory(const RadioPredefinedTextMessageFactory&);
		RadioPredefinedTextMessageFactory& operator=(const RadioPredefinedTextMessageFactory&);

        /**
        * Helper method to get operator key for this session
        */
        unsigned long getOperatorKey(const std::string& sessionId);


    private:
        
	    static RadioPredefinedTextMessageFactory* m_instance;
    };
}
#endif // !defined(RadioPredefinedTextMessageFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_)
