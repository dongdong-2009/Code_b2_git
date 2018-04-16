#if !defined(SequenceAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define SequenceAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/video_switch_agent/src/SequenceAccessFactory.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * SequenceAccessFactory is a singleton that is used to retrieve objects either from the
  * database or newly created. All objects returned will adhere to the ISequence interface.
  */

#include <vector>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

// Forward declarations

namespace TA_IRS_Core
{
    class ISequence;
    class IConfigSequence;
}

namespace TA_IRS_Core
{
	class SequenceAccessFactory
	{

	public:
        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      SequenceAccessFactory&
         *              A reference to an instance of an SequenceAccessFactory object.
         */

		static SequenceAccessFactory& getInstance();

        /**
         * ~SequenceAccessFactory
         *
         * Standard destructor.
         */

        virtual ~SequenceAccessFactory() {};

        /**
         * getSequence
         *
         * Retrieves the specified Sequence.
         *
         * @param       unsigned long sequenceKey 
         *              The database (entity) key to the Sequence to retrieve.
         * @param       bool isReadWrite
         *              True if the returned ISequence can be configured.
         *              False otherwise.
         *
         * @return      ISequence*
         *              A pointer to an object conforming to the ISequence interface.
         *
         *              N.B. It is the responsibility of the *client* to delete the returned 
         *              ISequence object.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   TA_Base_Core::DataException 
         *              Thrown if there is no Sequence matching the supplied key.
         */

        ISequence* getSequence( unsigned long sequenceKey, bool isReadWrite );

        /**
         * getAllSequences
         *
         * Retrieves the all the Sequences.
         *
         * @param       bool isReadWrite
         *              True if the returned ISequence can be configured.
         *              False otherwise.
         *
         * @return      std::vector< ISequence* >
         *              A vector of pointers to the objects conforming to the 
         *              ISequence interface.
         *
         *              N.B. It is the responsibility of the *client* to delete the returned 
         *              ISequence objects.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         */

        std::vector< ISequence* > getAllSequences( bool isReadWrite );

        /**
         * createSequence
         * 
         * Creates an empty IConfigSequence object, and returns a pointer to it.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IConfigSequence object.
         *
         * @return      IConfigSequence*
         *              A pointer to an IConfigSequence object.
         *
         * @param       unsigned long sequenceKey
         *              The database (entity) key for this Sequence.
         */

		IConfigSequence* createSequence( unsigned long sequenceKey );

	private:
		//
        // The one and only instance of this singleton class.
        //
		
        static SequenceAccessFactory* m_theClassInstance;

		//
        // Thread lock to protect singleton creation.
        //
        
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        /**
         * SequenceAccessFactory
         *
         * Standard constructor.
         *
         * This class is a singleton, so use getInstance() to get the one and only
         * instance of this class.
         */

		SequenceAccessFactory() {};

        //
        // Disable copy constructor and assignment operator.
        //

		SequenceAccessFactory( const SequenceAccessFactory& theSequenceAccessFactory );
		SequenceAccessFactory& operator=( const SequenceAccessFactory & );
	};

} // closes TA_IRS_Core

#endif // !defined(SequenceAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
