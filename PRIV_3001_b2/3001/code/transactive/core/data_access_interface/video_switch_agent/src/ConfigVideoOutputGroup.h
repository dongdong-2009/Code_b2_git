#if !defined(ConfigVideoOutputGroup_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigVideoOutputGroup_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/video_switch_agent/src/ConfigVideoOutputGroup.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * ConfigVideoOutputGroup is a concrete implementation of IConfigVideoOutputGroup, which is in turn 
  * an implementation of IConfigVideoOutputGroup. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for Type_Plugin_Matrix mappings.
  */

#include <map>
#include <memory>

#include "core/data_access_interface/video_switch_agent/src/IConfigVideoOutputGroup.h"
#include "core/data_access_interface/video_switch_agent/src/VideoOutputGroupHelper.h"

namespace TA_IRS_Core
{
    class ConfigVideoOutputGroup : public IConfigVideoOutputGroup
    {
    public:
        /**
         * ConfigVideoOutputGroup
         *
         * Standard constructer that creates a configurable VideoOutputGroup object with the given key.
         * This is used when creating a *new* VideoOutputGroup - that is, one that does not yet exist 
         * in the database.
         *
         * @param       unsigned long key 
         *              The database (entity) key of this VideoOutputGroup.
         */

        ConfigVideoOutputGroup( unsigned long key );

        /**
         * ~ConfigVideoOutputGroup
         *
         * Standard destructor.
         */

        virtual ~ConfigVideoOutputGroup() {};

        /**
         * getKey
         *
         * Returns the key for this VideoOutputGroup.
         *
         * Precondition:    Either - this VideoOutputGroup mapping was initially loaded from the database
         *                  OR     - writeVideoOutputGroupData() has already been called
         *                  AND    - deleteThisObject() has NOT been called
         *
         * @return          unsigned long
         *                  The database (entity) key for this VideoOutputGroup. 
         */

	    virtual unsigned long getKey();

        /**
         * getVideoOutputs
         *
         * Returns the VideoOutputs and their positions for this VideoOutputGroup.
         *
         * Precondition:    Either - this VideoOutputGroup mapping was initially loaded from the database
         *                  OR     - writeVideoOutputGroupData() has already been called
         *                  AND    - deleteThisObject() has NOT been called
         *
         * @return          std::map< unsigned short, unsigned long >
         *                  The VideoOutput keys referenced by their position in the VideoOutputGroup. 
         */

	    virtual std::map< unsigned short, unsigned long > getVideoOutputs();

        /**
         * setVideoOutputs
         *
         * Sets the VideoOutputs and their positions for this VideoOutputGroup.
         *
         * Precondition:    Either - this VideoOutputGroup mapping was initially loaded from the database
         *                  OR     - writeVideoOutputGroupData() has already been called
         *                  AND    - deleteThisObject() has NOT been called
         *
         * @param           const std::map< unsigned short, unsigned long >& videoOutputs
         *                  The VideoOutput keys referenced by their position in the VideoOutputGroup.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   TA_Base_Core::DataException 
         *              Thrown if there are multiple entries for the same position.
         */

        virtual void setVideoOutputs( const std::map< unsigned short, unsigned long >& videoOutputs );

        /**
         * invalidate
         *
         * Mark the data contained by this VideoOutputGroup as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    deleteThisObject() has NOT been called
         */

	    virtual void invalidate();

        /**
         * deleteThisObject
         *
	     * Removes this VideoOutputGroup mapping from the database. 
         * The calling application MUST then delete this VideoOutputGroup object, as it makes no sense
         * to keep it any longer.
         *
         * Precondition:    Either - this VideoOutputGroup mapping was initially loaded from the database
         *                  OR     - writeVideoOutputGroupData() has already been called
         *                  AND    - deleteThisObject() has NOT been called
         *
         * @exception   DatabaseException 
         *              Thrown if there is a problem accessing the data to the database.
         */

	    virtual void deleteThisObject();

        /**
         * applyChanges
         *
	     * This will apply all changes made to the database.
         *
         * Precondition:    deleteThisObject() has NOT been called
         *
         * @exception   DatabaseException 
         *              Thrown if there is a problem accessing the data to the database.
         * @exception   TA_Base_Core::DataException 
         *              Thrown if there is more than one VideoOutput per position.
         * @exception   DataConfigurationException 
         *              Thrown if the data contained in the VideoOutputGroup object
         *              is not sufficent to create an entry in the database.
	     */

	    virtual void applyChanges();
        
    private:
        //
        // Disable copy constructor and assignment operator.
        //

        ConfigVideoOutputGroup( const ConfigVideoOutputGroup& theConfigVideoOutputGroup );  
  		ConfigVideoOutputGroup& operator=( const ConfigVideoOutputGroup& );

        //
        // This is the object that does all the work.  It is a std::auto_ptr
        // so it will be automatically cleaned up on destruction of this object.
        //
        
        std::auto_ptr< VideoOutputGroupHelper > m_videoOutputGroupHelper;
    };

} // closes TA_IRS_Core

#endif // !defined(ConfigVideoOutputGroup_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
