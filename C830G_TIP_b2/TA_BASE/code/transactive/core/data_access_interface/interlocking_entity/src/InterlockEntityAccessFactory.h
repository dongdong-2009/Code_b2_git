/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking_entity/src/InterlockEntityAccessFactory.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#if !defined(InterlockEntityAccessFactory_H)
#define InterlockEntityAccessFactory_H

#include <string>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class IInterlockEntity;

    /**
     * Creates in memory cpoies of database records
     */
    class InterlockEntityAccessFactory
	{

	private:
		InterlockEntityAccessFactory() { };
		InterlockEntityAccessFactory( const InterlockEntityAccessFactory& theInterlockEntityAccessFactory);
		InterlockEntityAccessFactory& operator=(const InterlockEntityAccessFactory &);

	public:
        virtual ~InterlockEntityAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an InterlockEntityAccessFactory object.
         */
		static InterlockEntityAccessFactory& getInstance();


        /**
         * getInterlockEntity
         *
         * Retrieves the specified ddl name for the entity type and returns the associated data as an 
         * object conforming to the IInterlockEntity interface.
         *
         * @param entityType The database key to the interlocking rule to retrieve
         *
         * @return A pointer to an object conforming to the IInterlockEntity interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IInterlockEntity 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no rule
         *            matching the supplied PKEY, or if there is more than one rule
         *            matching the supplied PKEY. 
         */
		IInterlockEntity* getInterlockEntity(const std::string& entityType);


    private:
        static InterlockEntityAccessFactory* m_instance;
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
	};

} // closes TA_Base_Core

#endif // !defined(InterlockEntityAccessFactory_H)
