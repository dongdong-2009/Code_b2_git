/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by:  $Author: weikun.lin $
  * 
  * VideoTriggeringEventAccessFactory is a singleton that is used to retrieve VideoTriggeringEvent objects either from the
  * database or newly created. All VideoTriggeringEvent objects returned will adhear to the IVideoTriggeringEvent interface.
  */



#if !defined(VideoTriggeringEventAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define VideoTriggeringEventAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/video_switch_agent/src/IVideoTriggeringEvent.h"
#include "core/data_access_interface/video_switch_agent/src/IConfigVideoTriggeringEvent.h"




namespace TA_IRS_Core
{

	class VideoTriggeringEventAccessFactory
	{


	private:

        // Disable the following methods
		VideoTriggeringEventAccessFactory() { };
		VideoTriggeringEventAccessFactory( const VideoTriggeringEventAccessFactory& theVideoTriggeringEventAccessFactory);
		VideoTriggeringEventAccessFactory& operator=(const VideoTriggeringEventAccessFactory &);


	public:

      
        /** 
         * ~VideoTriggeringEventAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~VideoTriggeringEventAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an VideoTriggeringEventAccessFactory object.
         */
		static VideoTriggeringEventAccessFactory& getInstance();


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
         * getVideoTriggeringEvent
         *
         * Retrieves the specified VideoTriggeringEvent and returns the associated data as an 
         * object conforming to the IVideoTriggeringEvent interface.
         *
         * @param   key        the primary key/id of the VideoTriggeringEvent to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigVideoTriggeringEvent objects or set to false to return 
         *                     readonly IVideoTriggeringEvent objects.
         *
         * @return  A pointer to an object conforming to the IVideoTriggeringEvent interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IVideoTriggeringEvent object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  TA_Base_Core::DataException 
         *          - if there is no VideoTriggeringEvent matching the supplied key
         *          - if there is more than one VideoTriggeringEvent matching the supplied key. 
         *
         */
		IVideoTriggeringEvent* getVideoTriggeringEvent(const unsigned long key, const bool readWrite = false);

        /**
         * getVideoTriggeringEvent
         *
         * Retrieves the specified VideoTriggeringEvent and returns the associated data as an 
         * object conforming to the IVideoTriggeringEvent interface.
         *
         * @param   key        the primary key/id of the VideoTriggeringEvent to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigVideoTriggeringEvent objects or set to false to return 
         *                     readonly IVideoTriggeringEvent objects.
         *
         * @return  A pointer to an object conforming to the IVideoTriggeringEvent interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IVideoTriggeringEvent object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  TA_Base_Core::DataException 
         *          - if there is no VideoTriggeringEvent matching the supplied key
         *          - if there is more than one VideoTriggeringEvent matching the supplied key. 
         *
         */
		IVideoTriggeringEvent* getVideoTriggeringEvent(const unsigned long stationId, const unsigned long eventId, const bool readWrite = false);

        /**
         * getAllVideoTriggeringEvents
         *
         * Retrieves all VideoTriggeringEvents and returns the associated data as a vector of  
         * objects conforming to the IVideoTriggeringEvent interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigVideoTriggeringEvent objects or set to false to return 
         *                     readonly IVideoTriggeringEvent objects.
         *
         * @return  a vector of pointers to objects conforming to the IVideoTriggeringEvent interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IVideoTriggeringEvent object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  TA_Base_Core::DataException 
         *          - if there is no VideoTriggeringEvent matching the supplied key
         *          - if there is more than one VideoTriggeringEvent matching the supplied key. 
         *
         */
        IVideoTriggeringEvents getAllVideoTriggeringEvents( const bool readWrite = false );


        /**
         * getVideoTriggeringEventsWhere (UNTESTED)
         *
         * Gets all triggering events for the transactive location.
         *
         * @return A vector of pointers to objects conforming to the IVideoTriggeringEvent interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IVideoTriggeringEvent
         * objects
         *
         * @param   taLocation
         *          - The transactive location key to filter on.
         * @param   readWrite
         *          - true returns IConfigVideoTriggeringEvent objects
         *          - false returns IVideoTriggeringEvent objects
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IVideoTriggeringEvents getVideoTriggeringEventsByLocation(unsigned long taLocation, const bool readWrite = false);
 
        /**
         * createVideoTriggeringEvent
         * 
         * Creates an empty IConfigVideoTriggeringEvent object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigVideoTriggeringEvent object.
         *
         * @return A pointer to an IConfigVideoTriggeringEvent object
         *
         */
		IConfigVideoTriggeringEvent* createVideoTriggeringEvent();


        /**
         * copyVideoTriggeringEvent
         * 
         * Creates a new configuration VideoTriggeringEvent object using the VideoTriggeringEvent passed in
         *
         * @param  VideoTriggeringEventToCopy the ConfigVideoTriggeringEvent to copy
         *
         * @return the new object as a pointer to an IConfigVideoTriggeringEvent object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigVideoTriggeringEvent* copyVideoTriggeringEvent(const IConfigVideoTriggeringEvent* VideoTriggeringEventToCopy);


        /** 
          * pairExists
          *
          * This will return true if there is a record in the 
          * database that matches the given stationid, eventid pair. There will never be more
          * than one record that matches. Optionally the pkey of the current item can be excluded from the check.
          * ie if the current item is the matching one in the database then this isnt counted.
          *
          * @param  stationId   The station ID to check.
          * @param  eventId     The event ID to check.
          * @param  pkey        The pkey of the current item if applicable
          *
          * @return Whether the pair exists..
          *
          * @exception DatabaseException 
          *            - if there is a database problem.
          */
        bool pairExists(unsigned long stationId,
                        unsigned long eventId,
                        unsigned long pkey = -1);


    private:

        static VideoTriggeringEventAccessFactory* m_instance;
	};

} // closes TA_IRS_Core

#endif // !defined(VideoTriggeringEventAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
