/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/RightsAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * RightsAccessFactory is a singleton that is used to retrieve rights objects either from the
  * database or newly created. All rights objects returned will adhear to the IRights interface.
  *
  * These come from the SE_PROFILE_ACCESS database table.
  *
  */


#if !defined(RightsAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define RightsAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>
#include <map>

namespace TA_Base_Core
{
    class IConfigRights;
    class IRights;
	// yanrong ++
	typedef struct 
	{
		unsigned long profileKey;
		unsigned long subsystemStatekey;
	} GroupQueryParam;

	bool operator<(const GroupQueryParam& lp, const GroupQueryParam& rp);

	typedef std::map< GroupQueryParam, std::map< unsigned long, unsigned long > > ActionQueryParamMap;
	// ++ yanrong

	class RightsAccessFactory
	{

	public:
        virtual ~RightsAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an RightsAccessFactory object.
         */
		static RightsAccessFactory& getInstance();

		void getRightBySql(const std::string& sql, std::vector< IRights* >& rights, const bool readWrite);

        /**
         * getAllRights
         *
         * Retrieves all specified rights and returns the associated data as a vector of  
         * objects conforming to the IRights interface.
         *
         * @return A vector of pointers to objects conforming to the IRights interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IRights 
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
        std::vector<IRights*> getAllRights( const bool readWrite = false );

        /**
         * getRights
         *
         * Retrieves all specified rights and returns the associated data as a vector of  
         * objects conforming to the IRights interface.
         *
         * @return A vector of pointers to objects conforming to the IRights interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IRights 
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
        std::vector<IRights*> getRights( unsigned long subsystemStateKey, const bool readWrite = false );


        /**
         * createRights
         * 
         * Creates a new configuration rights object.
         *
         * @param unsigned long - The key of the profile this rights configuration is for
         * @param unsigned long - The key of the subsystem this rights configuration is for
         *
         * @return The configuration rights object as a pointer to an IConfigRights object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        IConfigRights* createRights(const long profileKey, const long subsystemKey,
			const long subsystemStateKey );

		//TD15533++
		unsigned long getActionGroup( 
			unsigned long profileKey,
			unsigned long subsystemKey,
			unsigned long subsystemStateKey );
		//++TD15533

		// yanrong ++
		// Query data from DB as many as possible at one time
		// to improve the launching speed of Duty Manager
		unsigned long getActionGroupEx( 
			unsigned long profileKey,
			unsigned long subsystemKey,
			unsigned long subsystemStateKey );
		// ++ yanrong
    private:
        //
        // These are private as this method is a singleton
        //
		RightsAccessFactory() {};
		RightsAccessFactory( const RightsAccessFactory& theRightsAccessFactory);
		RightsAccessFactory& operator=(const RightsAccessFactory &);

		static RightsAccessFactory* m_instance;
		// Yanrong ++
		ActionQueryParamMap m_queryStore;
		// ++ Yanrong
    };

} // closes TA_Base_Core

#endif // !defined(RightsAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
