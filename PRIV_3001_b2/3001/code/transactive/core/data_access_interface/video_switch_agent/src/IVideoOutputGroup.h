#if !defined(IVideoOutputGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IVideoOutputGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/video_switch_agent/src/IVideoOutputGroup.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * IVideoOutputGroup is an interface to an VideoOutputGroup mapping object. It allows the VideoOutputGroup 
  * mapping object implementation to be chagned (e.g. if necessary due to a database schema change) without 
  * changing code that uses it.
  */

#include <map>

namespace TA_IRS_Core
{
    class IVideoOutputGroup
    {
    public:
        /**
         * ~IVideoOutputGroup
         *
         * Standard destructor.
         */

        virtual ~IVideoOutputGroup() {};

        /**
         * getKey
         *
         * Returns the key for this VideoOutputGroup.
         *
         * Precondition:    0 != m_videoOutputGroupHelper.
         *
         * @return      unsigned long
         *              The database (entity) key for this VideoOutputGroup.
         */

	    virtual unsigned long getKey() = 0;

        /**
         * getVideoOutputs
         *
         * Returns the VideoOutputs and their positions within this VideoOutputGroup. 
         * The first position is 1.  If this is the first time data for this  
         * VideoOutputGroup has been requested, all the data is loaded from the database.
         *
         * Precondition:    0 != m_videoOutputGroupHelper.
         *
         * @return      std::map< unsigned short, unsigned long >
         *              The VideoOutput keys referenced by position.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   TA_Base_Core::DataException 
         *              Thrown if the data cannot be converted to the required format.
         */

	    virtual std::map< unsigned short, unsigned long > getVideoOutputs() = 0;

        /**
         * invalidate
         *
         * Mark the data contained by this VideoOutputGroup as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    0 != m_videoOutputGroupHelper.
         */

	    virtual void invalidate() = 0;
    };

} //close namespace TA_IRS_Core

#endif // !defined(IVideoOutputGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
