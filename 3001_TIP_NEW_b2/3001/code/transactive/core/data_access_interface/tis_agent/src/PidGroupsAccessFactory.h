#ifndef PID_GROUPS_ACCESS_FACTORY_H_INCLUDED
#define PID_GROUPS_ACCESS_FACTORY_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/tis_agent/src/PidGroupsAccessFactory.h $
  * @author christiandw
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by:  $Author: weikun.lin $
  * 
  * This class contains the database retrieval methods for named groups of PID's. 
  * The PidGroups returned are dependent on the location set via the setLocation( ) method.
  * The methods of this class ensure that setLocation( ) has been called at least once but this
  * class does not enforce that you call setLocation( ) in the event that the location associated
  * with your session changes - hence the client has this responcibility.
  *
  */

#include <string>
#include <vector>
#include <map>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_IRS_Core
{
   class IPidGroup;    // Forward declaration.
}

namespace TA_IRS_Core
{
    
    class PidGroupsAccessFactory
    {
    public:

        /**
         * ~PidGroupsAccessFactory
         *
         * dtor
         */

        virtual ~PidGroupsAccessFactory();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
		 *				
         * @return      PidGroupsAccessFactory&
         *              A reference to an instance of a PidGroupsAccessFactory object.
         */
		static PidGroupsAccessFactory& getInstance();

        /**
         * setLocation
         *
         * set the location related to the groups you will interact with. 
		 * Note that the intent is that you specify the location associated with your  current session ID.
		 * This information is used to determine which groups you have access to.
		 *
		 * You MUST call this prior to calling any other method.
		 * Note that it is the intent that you call this any time you recieve a session ID update notification
		 *
		 * Note this is a bit dangerous but I either set location once here or pass it as a param 
		 * in it in each and every call 
		 * Note that you can not get the location from Core because AuthenticationLib is now in Bus
		 * or I would have encapulated the relavent location resolution
         *
		 * @param		locationKey related to the PidGroups you wish to act on - the location key
		 * is compared to the value in the TI_PID_GROUPS.TIPGRO_ID database field
		 *				
         */
		void setLocation( unsigned long locationKey );

		unsigned long getLocation() {return m_locationKey;}


        /**
         * getPidGroups
         *
         * Retrieves all IPidGroups that the current location should see
         *
         * @return      std::vector<IPidGroup*>
         *              A vector of pointers to objects conforming to the IPidGroup interface.
         *
		 * PRECONDITION: setLocation( ) has been called at least once
		 *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if database interaction fails for any reason.
		 *         
         */
		std::vector<IPidGroup*> getPidGroups();

        /**
         * pidGroupExists
         *
         * @return true if a IPidGroup that you have access to (based on setLocation( ) value) already 
		 * exists with the specified name
		 *
		 * Updates local data if required.
         *
		 * @param		name
		 *				The name of the IPidGroup you want to check
		 *
		 * PRECONDITION: call will assert if name param is empty
		 * PRECONDITION: setLocation( ) has been called at least once
		 *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if database interaction fails for any reason.
         *             
         */
		bool pidGroupExists(const std::string& name);

        /**
         * createPidGroup
         *
         * create the specified IPidGroup. Local data and db are updated. 
		 * If a IPidGroup that you have access to (based on setLocation( ) value ) already 
		 * exists with the specified name it will be overwritten. 
		 * 
		 * Access control is not checked by this method
         *
		 * @param		name
		 *				The name of the IPidGroup you want to create
		 *
		 * @param		pidNames
		 *				std::vector<std::string> where each element is the entity name of a entity
		 *				of type datanode. The entity should represent a PID but this is not enforced
		 *
		 * PRECONDITION: call will assert if name param is empty
		 * PRECONDITION: call will assert if name param is empty or if any element in pidNames is empty
		 * PRECONDITION: setLocation( ) has been called at least once
		 *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if database interaction fails for any reason. Note that if any element
		 *				in pidNames is not a value in ENTITY.NAME then this exception will be thrown
         */
		void createPidGroup( const std::string& name,
							 std::vector<std::string> pidNames );

        /**
         * deletePidGroup
         *
         * delete the specified IPidGroup, if it exists, else do nothing. 
		 * Local data and db are updated.
		 * 
		 * Access control is not checked by this method
         *
		 * @param		name
		 *				The name of the IPidGroup you want to delete
		 *
		 *
		 * PRECONDITION: call will assert if name param is empty
		 * PRECONDITION: setLocation( ) has been called at least once
		 *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if database interaction fails for any reason. Note that
		 *				the specified group not existing is not considered an error (hence no exception)
         *             
         */
		void deletePidGroup( const std::string& name );

        /**
         * invalidate
         *
         * mark the local data as dirty. Data will be read from the db next time you call any
		 * of the above methods.
		 * Note that you should call this and retrieve that data this if you believe the data
		 * may have been modified by another process
         */
		void invalidate() { m_isDirty = true; }

    private: //data

		//the groups
		std::vector<IPidGroup*> m_groups;

		//the location key is compared to the value in the TI_PID_GROUPS.TIPGRO_ID database field
		unsigned long m_locationKey;
		//make sure the client sets the location at least once
		bool m_locationSet;

		//is the data dirty, i.e. needs to be read
		bool m_isDirty;

		static PidGroupsAccessFactory* m_theClassInstance;

        // Thread lock to protect singleton creation.    
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

		std::string m_localDatabase;

    private: //methods
        //
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        //
        PidGroupsAccessFactory();
        PidGroupsAccessFactory& operator=(const PidGroupsAccessFactory &) {};
        PidGroupsAccessFactory( const PidGroupsAccessFactory& ) {};


        /**
         * updateData m_groups data member by reading the database.
		 * 
		 *
		 *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if database interaction fails for any reason. Or if the specified
		 *				IPidGroup does not exist in the groups that you have access to
         *             
         */
		void updateData();

        /**
         * readGroupTable
         *
         * read the relavent data from the TI_PID_GROUPS table
         *
		 * @return a map of group names and their keys, where the location
		 *
		 * PRECONDITION: setLocation( ) has been called at least once
		 *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if database interaction fails for any reason. Or if the specified
		 *				IPidGroup does not exist in the groups that you have access to
         *             
         */
		std::map< std::string, std::string > readGroupTable();


        /**
         * readPidGroupMapTable
         *
         * read the data for the specified group from the TI_PID_GROUP_MAP table.
		 * In fact, the read extracts the entity name of the PID from the entity table
         *
		 * @return a vector of entity names of the PID's in the specified group
		 *
		 *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if database interaction fails for any reason.
		 *        
         */
		std::vector< std::string > readPidGroupMapTable( std::string groupKey );

        /**
         * return the location key set via setLocation( )
		 *
		 * PRECONDITION: setLocation( ) has been called at least once            
         */
		unsigned long getLocationKey();

        /**
         * deletePidGroup
         *
         * delete the specified IPidGroup, if it exists, else do nothing
		 * Does NOT update local data
         * Access control is not checked by this method
		 *
		 * @param		name
		 *				The name of the IPidGroup you want to delete
		 *
		 *
		 * PRECONDITION: call will assert if name param is empty
		 *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if database interaction fails for any reason. Note that
		 *				the specified group not existing is not considered an error (hence no exception)
         *             
         */
		void removeGroupFromTables( const std::string& name );

        /**
         * addGroupToTables
         *
         * persist the specified IPidGroup. 
		 * Does NOT update local data
		 * Access control is not checked by this method
         *
		 * @param		name
		 *				The name of the IPidGroup you want to add to the database
		 *
		 * @param		pidNames
		 *				std::vector<std::string> where each element is the entity name of a entity
		 *				of type datanode. The entity should represent a PID but this is not enforced
		 *
		 * PRECONDITION: call will assert if name param is empty
		 * PRECONDITION: call will assert if name param is empty or if any element in pidNames is empty
		 * PRECONDITION: setLocation( ) has been called at least once
		 *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if database interaction fails for any reason. Or If a 
		 *				IPidGroup that you have access to (based on setLocation( ) ) already exists with this name.
		 *				Note that if any element in pidNames is not a value in ENTITY.NAME then 
		 *				this exception will be thrown
         *             
         */
		void addGroupToTables( const std::string& name, std::vector<std::string> pidNames );

        /**
         * getPidKeysFromNames
         *
         * gets the pkeys of the specified PID entities. Returns empty vector if pidNames is empty
         *
		 *
		 * @param		pidNames
		 *				std::vector<std::string> where each element is the entity name of a entity
		 *				of type datanode. The entity should represent a PID but this is not enforced
		 *
		 * PRECONDITION: call will assert if any element in pidNames is empty 
		 *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if database interaction fails for any reason or if any element in pidNames is
		 *				not the name of a entity of type Datanode
         *             
         */
		std::vector<unsigned long> getPidKeysFromNames( std::vector<std::string> pidNames );

        /**
         * getNextPkeyForGroup
         *
         * return a pkey that is valid to use when adding a record to the TI_PID_GROUPS table
         *
		 *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if database interaction fails for any reason. 
         *             
         */
		std::string getNextPkeyForGroup();

		std::string getLocalDatabaseName();

    };

} // end namespace TA_IRS_Core

#endif
