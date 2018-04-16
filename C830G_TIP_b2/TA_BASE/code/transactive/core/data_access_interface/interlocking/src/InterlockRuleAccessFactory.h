/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking/src/InterlockRuleAccessFactory.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#if !defined(InterlockRuleAccessFactory_H)
#define InterlockRuleAccessFactory_H

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{

    class IInterlockRule;
    class IConfigInterlockRule;

    /**
     *  Creates in memory versions of InterlockingRules as required
     */
    class InterlockRuleAccessFactory
	{

	private:
		InterlockRuleAccessFactory() { };
		InterlockRuleAccessFactory( const InterlockRuleAccessFactory& theInterlockRuleAccessFactory);
		InterlockRuleAccessFactory& operator=(const InterlockRuleAccessFactory &);

	public:
        virtual ~InterlockRuleAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an InterlockRuleAccessFactory object.
         */
		static InterlockRuleAccessFactory& getInstance();


        /**
         * getInterlockRule
         *
         * Retrieves the specified interlocking rule and returns the associated data as an 
         * object conforming to the IInterlockRule interface.
         *
         * @param key The database key to the interlocking rule to retrieve
         *
         * @return A pointer to an object conforming to the IInterlockRule interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IInterlockRule 
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
		IInterlockRule* getInterlockRule(const unsigned long key, const bool readWrite);

        /**
         * getInterlockRules
         *
         * Retrieves all interlocking rules for an entity and returns the associated data 
         * as a vector of objects conforming to the IObject interface.
         *
         * @return A vector of pointers to objects conforming to the IInterlockRule interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IInterlockRule
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException  
         */
        std::vector<IInterlockRule*> getInterlockRules(const unsigned long entity, const std::string& entityType, const bool readWrite = false );

        /**
         * getAllInterlockRules
         *
         * Retrieves all interlocking rules and returns the associated data as a vector of  
         * objects conforming to the IObject interface.
         *
         * @return A vector of pointers to objects conforming to the IInterlockRule interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IInterlockRule
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException  
         */
        std::vector<IInterlockRule*> getAllInterlockRules( const bool readWrite = false );

        /**
         * createInterlockRule
         * 
         * Creates an empty IConfigInterlockRule object, and returns a pointer to it.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IConfigInterlockRule object
         *
         * @return A pointer to an IConfigInterlockRule object
         *
         */
		IInterlockRule* createInterlockRule();

        /**
         * copyInterlockRule
         * 
         * Creates a new configuration operator object using the operator passed in
         *
         * @param IConfigOperator* The operator to copy
         *
         * @return The configuration operator object as a pointer to an IConfigOperator object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IInterlockRule* copyInterlockRule(const IConfigInterlockRule* ruleToCopy);

    private:
        static InterlockRuleAccessFactory* m_instance;
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
	};

} // closes TA_Base_Core

#endif // !defined(InterlockRuleAccessFactory_H)
