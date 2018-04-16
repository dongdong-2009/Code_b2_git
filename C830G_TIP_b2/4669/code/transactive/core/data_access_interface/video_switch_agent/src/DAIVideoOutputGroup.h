#if !defined(DAIVideoOutputGroup_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define DAIVideoOutputGroup_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/video_switch_agent/src/DAIVideoOutputGroup.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * DAIVideoOutputGroup is an implementation of IDAIVideoOutputGroup. It holds the data specific to a 
  * DAIVideoOutputGroup mapping entry in the database, and allows read-only access to that data.
  */

#include <vector>
#include <memory>

#include "core/data_access_interface/video_switch_agent/src/VideoOutputGroupHelper.h"
#include "core/data_access_interface/video_switch_agent/src/IVideoOutputGroup.h"

namespace TA_IRS_Core
{
    namespace TA_VideoSwitchAgentDAI    // Need to namespace this o/w we get clashes with the entity_access/src/DAIVideoOutputGroup.
    {
        class DAIVideoOutputGroup : public IVideoOutputGroup
        {
        public:

            /**
             * DAIVideoOutputGroup
             *
             * Standard constructer that creates a DAIVideoOutputGroup object with the given key.
             *
             * @param       unsigned long key 
             *              The database (entity) key of this DAIVideoOutputGroup.
             */

            DAIVideoOutputGroup( unsigned long key );

            /**
             * ~DAIVideoOutputGroup
             *
             * Standard destructor.
             */

            virtual ~DAIVideoOutputGroup() {};

            /**
             * getKey
             *
             * Returns the key for this DAIVideoOutputGroup.
             *
             * Precondition:    0 != m_videoOutputGroupHelper.
             *
             * @return      unsigned long
             *              The database (entity) key for this DAIVideoOutputGroup.
             */

	        virtual unsigned long getKey();

            /**
             * getVideoOutputs
             *
             * Returns the VideoOutputs and their positions within this DAIVideoOutputGroup. 
             * The first position is 1.  If this is the first time data for this  
             * DAIVideoOutputGroup has been requested, all the data is loaded from the database.
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

	        virtual std::map< unsigned short, unsigned long > getVideoOutputs();

            /**
             * invalidate
             *
             * Mark the data contained by this DAIVideoOutputGroup as invalid. The next call to get...() 
             * following a call to invalidate() will cause all the data to be reloaded from
             * the database.
             *
             * Precondition:    0 != m_videoOutputGroupHelper.
             */

	        virtual void invalidate();

        private:
            //
            // Disable copy constructor and assignment operator.
            //

            DAIVideoOutputGroup( const DAIVideoOutputGroup& theDAIVideoOutputGroup);  
  		    DAIVideoOutputGroup& operator=( const DAIVideoOutputGroup& );

            //
            // This is the object that does all the work.  It is a std::auto_ptr
            // so it will be automatically cleaned up on destruction of this object.
            //
        
            std::auto_ptr< VideoOutputGroupHelper > m_videoOutputGroupHelper;
        };

    } // closes TA_DAIVideoOutputGroupDAI

} // closes TA_IRS_Core

#endif // !defined(DAIVideoOutputGroup_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
