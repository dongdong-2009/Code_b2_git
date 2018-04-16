/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking_entity/src/InterlockVariableAccessFactory.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#if !defined(InterlockVariableAccessFactory_H)
#define InterlockVariableAccessFactory_H

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/interlocking_variable/src/IInterlockVariable.h"

namespace TA_Base_Core
{

	class InterlockVariableAccessFactory
	{

	private:
		InterlockVariableAccessFactory() { };
		InterlockVariableAccessFactory( const InterlockVariableAccessFactory& theInterlockVariableAccessFactory);
		InterlockVariableAccessFactory& operator=(const InterlockVariableAccessFactory &);

	public:
        virtual ~InterlockVariableAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an InterlockVariableAccessFactory object.
         */
		static InterlockVariableAccessFactory& getInstance();


        /**
         * getInterlockVariable
         *
         * Retrieves the specified ddl name for the variable type and returns the associated data as an 
         * object conforming to the IInterlockVariable interface.
         *
         * @param entityType The database key to the variable type to retrieve
         *
         * @return A pointer to an object conforming to the IInterlockVariable interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IInterlockVariable 
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
        IInterlockVariable* getInterlockVariable(const std::string& variableType);

        /**
         * getInterlockVariables
         *
         * Retrieves the specified ddl names for all variable types and returns the associated data as a
         * list of objects conforming to the IInterlockVariable interface.
         *
         * @return A list of pointers to objects conforming to the IInterlockVariable interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IInterlockVariables
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
        std::vector<IInterlockVariable*> getInterlockVariables();


    private:
        static InterlockVariableAccessFactory* m_instance;
	};

} // closes TA_Base_Core

#endif // !defined(InterlockVariableAccessFactory_H)
