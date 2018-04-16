#if !defined(ConfigSequence_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigSequence_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/video_switch_agent/src/ConfigSequence.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * ConfigSequence is a concrete implementation of IConfigSequence, which is in turn 
  * an implementation of IConfigSequence. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for Sequences.
  *
  * Note: The dwell time for Sequences can be retrieved/set using the 
  * Sequence object accessed through the EntityAccessFactory.
  */

#include <map>
#include <memory>

#include "core/data_access_interface/video_switch_agent/src/IConfigSequence.h"
#include "core/data_access_interface/video_switch_agent/src/SequenceHelper.h"

namespace TA_IRS_Core
{
    class ConfigSequence : public IConfigSequence
    {
    public:
        /**
         * ConfigSequence
         *
         * Standard constructer that creates a configurable Sequence object with the given key.
         * This is used when creating a *new* Sequence - that is, one that does not yet exist 
         * in the database.
         *
         * @param       unsigned long key 
         *              The database (entity) key of this Sequence.
         */

        ConfigSequence( unsigned long key );

        /**
         * ~ConfigSequence
         *
         * Standard destructor.
         */

        virtual ~ConfigSequence() {};

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

        virtual void setVideoInputs( const std::map< unsigned short, unsigned long >& videoInputs );

        /**
         * invalidate
         *
         * Mark the data contained by this Sequence as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    deleteThisObject() has NOT been called
         */

	    virtual void invalidate();

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
         *              Thrown if the data contained in the Sequence object
         *              is not sufficent to create an entry in the database.
	     */

	    virtual void applyChanges();
        
    private:
        //
        // Disable copy constructor and assignment operator.
        //

        ConfigSequence( const ConfigSequence& theConfigSequence );  
  		ConfigSequence& operator=( const ConfigSequence & );

        //
        // This is the object that does all the work.  It is a std::auto_ptr
        // so it will be automatically cleaned up on destruction of this object.
        //
        
        std::auto_ptr< SequenceHelper > m_sequenceHelper;
    };

} // closes TA_IRS_Core

#endif // !defined(ConfigSequence_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
