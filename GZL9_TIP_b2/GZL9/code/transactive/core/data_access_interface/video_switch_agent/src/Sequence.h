#if !defined(Sequence_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define Sequence_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/video_switch_agent/src/Sequence.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Sequence is an implementation of ISequence. It holds the data specific to a 
  * Sequence mapping entry in the database, and allows read-only access to that data.
  *
  * Note: The dwell time for Sequences can be retrieved using the 
  * Sequence object accessed through the EntityAccessFactory.
  */

#include <vector>
#include <memory>

#include "core/data_access_interface/video_switch_agent/src/SequenceHelper.h"
#include "core/data_access_interface/video_switch_agent/src/ISequence.h"

namespace TA_IRS_Core
{
    namespace TA_VideoSwitchAgentDAI    // Need to namespace this o/w we get clashes with the entity_access/src/Sequence.
    {
        class Sequence : public ISequence
        {
        public:
            /**
             * Sequence
             *
             * Standard constructer that creates a Sequence object with the given key.
             *
             * @param       unsigned long key 
             *              The database (entity) key of this Sequence.
             */

            Sequence( unsigned long key );

            /**
             * ~Sequence
             *
             * Standard destructor.
             */

            virtual ~Sequence() {};

            /**
             * getKey
             *
             * Returns the key for this Sequence.
             *
             * Precondition:    0 != m_videoOutputGroupHelper.
             *
             * @return      unsigned long
             *              The database (entity) key for this Sequence.
             */

	        virtual unsigned long getKey();

            /**
             * getVideoInputs
             *
             * Returns the VideoInputs and their positions for this Sequence.
             *
             * Precondition:    Either - this Sequence mapping was initially loaded from the database
             *                  OR     - writeSequenceData() has already been called
             *                  AND    - deleteThisObject() has NOT been called
             *
             * @return          std::map< unsigned short, unsigned long >
             *                  The VideoOutput keys referenced by their position in the Sequence. 
             */

	        virtual std::map< unsigned short, unsigned long > getVideoInputs();

            /**
             * invalidate
             *
             * Mark the data contained by this Sequence as invalid. The next call to get...() 
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

            Sequence( const Sequence& theSequence);  
  		    Sequence& operator=(const Sequence &);

            //
            // This is the object that does all the work.  It is a std::auto_ptr
            // so it will be automatically cleaned up on destruction of this object.
            //
        
            std::auto_ptr< SequenceHelper > m_sequenceHelper;
        };

    } // closes TA_SequenceDAI

} // closes TA_IRS_Core

#endif // !defined(Sequence_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
