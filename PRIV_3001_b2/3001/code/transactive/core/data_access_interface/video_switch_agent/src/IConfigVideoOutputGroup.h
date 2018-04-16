#if !defined(IConfigVideoOutputGroup_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigVideoOutputGroup_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/video_switch_agent/src/IConfigVideoOutputGroup.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * IConfigVideoOutputGroup - This is the interface supported by objects that allow configuration of
  * VideoOutputGroups. It provides methods for retrieving, settings, and writing data for a 
  * VideoOutputGroup object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#include <map>

#include "core/data_access_interface/video_switch_agent/src/IVideoOutputGroup.h"

namespace TA_IRS_Core
{
    class IConfigVideoOutputGroup : public IVideoOutputGroup
    {
    public:
        /**
         * ~IConfigVideoOutputGroup
         *
         * Standard destructor.
         */

        virtual ~IConfigVideoOutputGroup() {};

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

	    virtual unsigned long getKey() = 0;

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

	    virtual std::map< unsigned short, unsigned long > getVideoOutputs() = 0;

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

        virtual void setVideoOutputs( const std::map< unsigned short, unsigned long >& videoOutputs ) = 0;

        /**
         * invalidate
         *
         * Mark the data contained by this VideoOutputGroup as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    deleteThisObject() has NOT been called
         */

	    virtual void invalidate() = 0;

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

	    virtual void deleteThisObject() = 0;

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

	    virtual void applyChanges() = 0;
    };

} //close namespace TA_IRS_Core

#endif // !defined(IConfigVideoOutputGroup_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
