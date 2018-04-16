#if !defined(IConfigSequence_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigSequence_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/video_switch_agent/src/IConfigSequence.h $
  * @author Katherine Thomson
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by: $Author: weikun.lin $
  * 
  * IConfigSequence - This is the interface supported by objects that allow configuration of
  * Sequences. It provides methods for retrieving, settings, and writing data for a 
  * Sequence object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  *
  * Note: The dwell time for Sequences can be retrieved/set using the 
  * Sequence object accessed through the EntityAccessFactory.
  */

#include <map>

#include "core/data_access_interface/video_switch_agent/src/ISequence.h"

namespace TA_IRS_Core
{
    class IConfigSequence : public ISequence
    {
    public:
        /**
         * ~IConfigSequence
         *
         * Standard destructor.
         */

        virtual ~IConfigSequence() {};

        /**
         * getKey
         *
         * Returns the key for this Sequence.
         *
         * Precondition:    Either - this Sequence mapping was initially loaded from the database
         *                  OR     - writeSequenceData() has already been called
         *                  AND    - deleteThisObject() has NOT been called
         *
         * @return          unsigned long
         *                  The database (entity) key for this Sequence. 
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
         * setVideoInputs
         *
         * Sets the VideoInputs and their positions for this Sequence.
         *
         * Precondition:    Either - this Sequence mapping was initially loaded from the database
         *                  OR     - writeSequenceData() has already been called
         *                  AND    - deleteThisObject() has NOT been called
         *
         * @param           const std::map< unsigned short, unsigned long >& videoInputs
         *                  The VideoOutput keys referenced by their position in the Sequence.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   TA_Base_Core::DataException 
         *              Thrown if there are multiple entries for the same position.
         */

        virtual void setVideoInputs( const std::map< unsigned short, unsigned long >& videoInputs ) = 0;

        /**
         * invalidate
         *
         * Mark the data contained by this Sequence as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    deleteThisObject() has NOT been called
         */

	    virtual void invalidate() = 0;

        /**
         * deleteThisObject
         *
	     * Removes this Sequence mapping from the database. 
         * The calling application MUST then delete this Sequence object, as it makes no sense
         * to keep it any longer.
         *
         * Precondition:    Either - this Sequence mapping was initially loaded from the database
         *                  OR     - writeSequenceData() has already been called
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
         *              Thrown if the data contained in the Sequence object
         *              is not sufficent to create an entry in the database.
	     */

	    virtual void applyChanges() = 0;
    };

} //close namespace TA_IRS_Core

#endif // !defined(IConfigSequence_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
