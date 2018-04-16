#if !defined(VideoOutputGroupAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define VideoOutputGroupAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/video_switch_agent/src/VideoOutputGroupAccessFactory.h $
  * @author Katherine Thomson
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by: $Author: weikun.lin $
  * 
  * VideoOutputGroupAccessFactory is a singleton that is used to retrieve objects either from the
  * database or newly created. All objects returned will adhere to the IVideoOutputGroup interface.
  */

#include <vector>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

// Forward declarations

namespace TA_IRS_Core
{
    class IVideoOutputGroup;
    class IConfigVideoOutputGroup;
}

namespace TA_IRS_Core
{
	class VideoOutputGroupAccessFactory
	{

	public:
        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      VideoOutputGroupAccessFactory&
         *              A reference to an instance of an VideoOutputGroupAccessFactory object.
         */

		static VideoOutputGroupAccessFactory& getInstance();

        /**
         * ~VideoOutputGroupAccessFactory
         *
         * Standard destructor.
         */

        virtual ~VideoOutputGroupAccessFactory() {};

        /**
         * getVideoOutputGroup
         *
         * Retrieves the specified VideoOutputGroup.
         *
         * @param       unsigned long videoOutputGroupKey 
         *              The database (entity) key to the VideoOutputGroup to retrieve.
         * @param       bool isReadWrite
         *              True if the returned IVideoOutputGroup can be configured.
         *              False otherwise.
         *
         * @return      IVideoOutputGroup*
         *              A pointer to an object conforming to the IVideoOutputGroup interface.
         *
         *              N.B. It is the responsibility of the *client* to delete the returned 
         *              IVideoOutputGroup object.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   TA_Base_Core::DataException 
         *              Thrown if there is no VideoOutputGroup matching the supplied key.
         */

        IVideoOutputGroup* getVideoOutputGroup( unsigned long videoOutputGroupKey, bool isReadWrite );

        /**
         * getAllVideoOutputGroups
         *
         * Retrieves the all the VideoOutputGroups.
         *
         * @param       bool isReadWrite
         *              True if the returned IVideoOutputGroup can be configured.
         *              False otherwise.
         *
         * @return      std::vector< IVideoOutputGroup* >
         *              A vector of pointers to the objects conforming to the 
         *              IVideoOutputGroup interface.
         *
         *              N.B. It is the responsibility of the *client* to delete the returned 
         *              IVideoOutputGroup objects.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         */

        std::vector< IVideoOutputGroup* > getAllVideoOutputGroups( bool isReadWrite );

        /**
         * createVideoOutputGroup
         * 
         * Creates an empty IConfigVideoOutputGroup object, and returns a pointer to it.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IConfigVideoOutputGroup object.
         *
         * @return      IConfigVideoOutputGroup*
         *              A pointer to an IConfigVideoOutputGroup object.
         *
         * @param       unsigned long videoOutputGroupKey
         *              The database (entity) key for this VideoOutputGroup.
         */

		IConfigVideoOutputGroup* createVideoOutputGroup( unsigned long videoOutputGroupKey );

	private:
		//
        // The one and only instance of this singleton class.
        //
		
        static VideoOutputGroupAccessFactory* m_theClassInstance;

		//
        // Thread lock to protect singleton creation.
        //
        
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        /**
         * VideoOutputGroupAccessFactory
         *
         * Standard constructor.
         *
         * This class is a singleton, so use getInstance() to get the one and only
         * instance of this class.
         */

		VideoOutputGroupAccessFactory() {};

        //
        // Disable copy constructor and assignment operator.
        //

		VideoOutputGroupAccessFactory( const VideoOutputGroupAccessFactory& theVideoOutputGroupAccessFactory );
		VideoOutputGroupAccessFactory& operator=( const VideoOutputGroupAccessFactory& );
	};

} // closes TA_IRS_Core

#endif // !defined(VideoOutputGroupAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
