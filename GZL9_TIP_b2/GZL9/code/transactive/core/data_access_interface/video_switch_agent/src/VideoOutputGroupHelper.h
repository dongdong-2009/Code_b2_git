#if !defined(VideoOutputGroupHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define VideoOutputGroupHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/video_switch_agent/src/VideoOutputGroupHelper.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * VideoOutputGroupHelper is an object that is held by VideoOutputGroup and ConfigVideoOutputGroup objects. 
  * It contains all data used by VideoOutputGroups, and manipulation
  * methods for the data. It helps avoid re-writing code for both VideoOutputGroup and ConfigVideoOutputGroup.
  */

#pragma warning ( disable : 4786 )

#include <map>
#include <climits>

namespace TA_IRS_Core
{
    class VideoOutputGroupHelper
    {
    public:
        /**
         * VideoOutputGroupHelper
         * 
         * This constructor creates a new VideoOutputGroupHelper for configuration.
         */

 //       VideoOutputGroupHelper();
   
       /**
         * VideoOutputGroupHelper
         * 
         * This constructor creates a new VideoOutputGroupHelper for the specified key.
         *
         * @param       unsigned long videoOutputGroupKey
         *              The database (entity) key for this VideoOutputGroup.
         */

        VideoOutputGroupHelper( unsigned long videoOutputGroupKey );

        /**
          * VideoOutputGroupHelper
          *
          * Standard copy constructor.
          */

        VideoOutputGroupHelper( const VideoOutputGroupHelper& theVideoOutputGroupHelper );

        /**
         * ~VideoOutputGroupHelper
         *
         * Standard destructor.
         */

        virtual ~VideoOutputGroupHelper() {};

        /**
         * getKey
         *
         * Returns the VideoOutputGroup key for this VideoOutputGroup entry.
         *
         * Preconditions:   Either - this VideoOutputGroup entry was initially 
         *                           loaded from the database
         *                  OR     - writeVideoOutputGroupData() has already been called
         *
         * @return      unsigned long
         *              The key for this VideoOutputGroup.
         */

	    unsigned long getKey();

        /**
         * getVideoOutputs
         *
         * Returns the VideoOutput keys and their positions in the VideoOutputGroup.
         *
         * Preconditions:   Either - this VideoOutputGroup entry was initially 
         *                           loaded from the database
         *                  OR     - writeVideoOutputGroupData() has already been called
         *
         * @return      std::map< unsigned short, unsigned long >
         *              The VideoOutput keys referenced by their position in the VideoOutputGroup.
         */

	    std::map< unsigned short, unsigned long > getVideoOutputs();

        /**
         * setVideoOutputs
         *
         * Sets the VideoOutputs and their positions for this VideoOutputGroup.
         * This should only be used by the ConfigVideoOutputGroup class.
         *
         * @param       const std::map< unsigned short, unsigned long >& videoOutputs
         *              The VideoOutput keys referenced by their position in the VideoOutputGroup.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   TA_Base_Core::DataException 
         *              Thrown if there are multiple entries for the same position.
         */

        void setVideoOutputs( const std::map< unsigned short, unsigned long >& videoOutputs );

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

        /**
         * writeVideoOutputGroupData
         * 
         * Write this VideoOutputGroup to the database.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   TA_Base_Core::DataException 
         *              Thrown if:
         *              1) There are multiple entries for the same position.
         * @exception   DataConfigurationException 
         *              Thrown if the data contained in the VideoOutputGroup object
         *              is not sufficent to create an entry in the database.
         */

        void writeVideoOutputGroupData();

        /**
         * deleteVideoOutputGroup
         *
         * Remove this VideoOutputGroup from the database. Once this method has been executed, the
         * VideoOutputGroupHelper object will have been destroyed.
         *
         * Precondition:    Either - this VideoOutputGroup was initially loaded from the database
         *                  OR     - writeVideoOutputGroupData() has already been called.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         */

        void deleteVideoOutputGroup();

    private:
        //
        // Disable the assignment operator.
        //

		VideoOutputGroupHelper& operator=( const VideoOutputGroupHelper& );
        
        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * Precondition:    Either - this VideoOutputGroup was initially loaded from the database
         *                  OR     - writeVideoOutputGroupData() has already been called.
         */

	    void reload();

        /**
         * insertNewVideoOutputGroup
         *
         * Inserts a new VideoOutputGroup into the database.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   TA_Base_Core::DataException
         *              Thrown if the VideoOutputGroup already exists in the database.
         */

        void insertNewVideoOutputGroup();

        /**
         * updateExistingVideoOutputGroup
         *
         * Updates an existing VideoOutputGroup in the database.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   TA_Base_Core::DataException
         *              Thrown if the VideoOutputGroup does not already exist in the database.
         */

        void updateExistingVideoOutputGroup();

        //
        // Flag to indicate if the data needs reloading.
        //

	    bool m_isValidData;

        //
        // Flag to indicate if this is a new VideoOutputGroup or an existing one
        // loaded from the database.
        //

        bool m_isNew;
        
        //
        // The VideoOutputGroup database (entity) key and the keys of the VideoOutputs in this 
        // VideoOutputGroup referenced by position.
        //
        unsigned long m_key;
        std::map< unsigned short, unsigned long > m_videoOutputs;
    };

} // closes TA_IRS_Core

#endif // !defined(VideoOutputGroupHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
