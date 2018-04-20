#if !defined(ISequence_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define ISequence_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/video_switch_agent/src/ISequence.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ISequence is an interface to an Sequence mapping object. It allows the Sequence 
  * mapping object implementation to be chagned (e.g. if necessary due to a database schema change) without 
  * changing code that uses it.
  *
  * Note: The dwell time for Sequences can be retrieved using the 
  * Sequence object accessed through the EntityAccessFactory.
  */

#include <map>

namespace TA_IRS_Core
{
    class ISequence
    {
    public:
        /**
         * ~ISequence
         *
         * Standard destructor.
         */

        virtual ~ISequence() {};

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

	    virtual unsigned long getKey() = 0;

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

	    virtual std::map< unsigned short, unsigned long > getVideoInputs() = 0;

        /**
         * invalidate
         *
         * Mark the data contained by this Sequence as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    0 != m_videoOutputGroupHelper.
         */

	    virtual void invalidate() = 0;
    };

} //close namespace TA_IRS_Core

#endif // !defined(ISequence_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
