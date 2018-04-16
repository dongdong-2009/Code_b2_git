/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/database/src/OCIConnection.h $
* @author:  Ripple
* @version: $Revision: #2 $
*
* Last modification: $DateTime: 2011/03/29 13:47:32 $
* Last modified by:  $Author: builder $
*
* This class is used by OCIAccess to store database connection 
* information and maintain the associated connection handles.  
* If the database automatically reconnects a database with the same name, username and 
* password, this class will connect to the new database on the first exec
* failure.
* 
*/

#ifndef OCICONNECTION_H
#define OCICONNECTION_H

#ifdef WIN32
#pragma warning(disable: 4290) // C++ exception specification ignored
#endif // WIN32

#include <string>
#include <map>
#include <omnithread.h>  // This must be included before oci.h to avoid redefinition errors.
// It is usually located in $ORACLE_HOME/rdbms/demo
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "CommonType.h"
#include "DBException.h"

namespace ocl
{
	class OraConnection;
	class OraQuery;
	class OraTransaction;
}

namespace TA_Base_Core
{
    class QueryAdapter;

	//TD18251, jianghp, to replace the OCI with OCL
	class OCIConnection
	{
	public:
		// Various errors we are interested in.
		enum 
		{
			UNIQUE_CONSTRAINT_VIOLATED_ERROR =     1,
				SESSION_KILLED                   =    12,
				NOT_LOGGED_IN                    =  1012,
				COLUMN_NULL_ERROR                =  1405,
				END_OF_FILE_ON_COMMS_CHANNEL     =  3113,
				NO_LISTENER                      = 12541,
				NO_SUCH_DATABASE                 = 12545,
				ALREADY_CONNECTED_ERROR          = 24309,
				ALREADY_AUTHENTICATED            = 24313,
				REQUIRE_EXPLICT_ATTACH           = 24327
		};		
		/**
		* OCIConnection
		* 
		* Sole constructor that stores the database connection details and 
		* allocates handles to perform transactions with the database.
		* Note: this method can be unsuccessful, so checkIsValid() should be 
		* called afterwards to check that construction was successful.
		* 
		* @param connectionStr databaseName:userName:password:hostName
		* @param databaseName  the name of the database to connect to
		* @param userName      the name of the user for the session
		* @param password      the name of the password for the session
		* @param hostName      the name of the host that the database is on
		*/
		OCIConnection( const std::string& connectionStr, 
			const std::string& databaseName, 
			const std::string& userName, 
			const std::string& password, 
			const std::string& hostName = "" );
		
		
			/**
			* ~OCIConnection
			* 
			* Sole standard destructor. Frees the handles allocated in the constructor.
		*/
		~OCIConnection();

		// Indicates that there may be more data
		//bool hasMore() const;		
        bool isAnyQueryActive( unsigned long ulMaxTimeoutInSecs );

		//bool isActive() const;

		/**
		* getConnectionString
		*
		* Returns the connection string for this OCIConnection.
		*
		* @return std::string  the connection string in the form
		*                      databaseName:userName:password:hostName
		*/
		std::string getConnectionString() const;
		
		/**
		* open
		*
		* Starts a new session with the database if there is not one already 
		* running.
		*
		* @exception DBException Thrown if there is an OCI error.
		*/
		void open();
		
		bool isOpen();
		/**
		* close
		*
		* Ends the session with the database if there is one running.
		*
		* @exception DBException Thrown if there is an OCI error.
		*/
		void close();
		
		
		/**
		* closeAll
		*
		* Ends all sessions with the database if there are any running.
		*
		* @exception DBException Thrown if there is an OCI error.
		*/
		void closeAll();
		
		
		/**
		* exec
		*
		* Executes the given statement on the database.  This method
		* handles both queries (e.g. "SELECT...") and non-queries (e.g."INSERT...")
		*
		* @param statement    the statement to execute
		* @param isQuery      a bool that is true if the statement is a query
		* @param numColumns   the number of columns to retrieve
		* @param bindType     the type of binding required
		* @exception DBException Thrown if there is an OCI error.
        */
        //void exec( const std::string& statement, ocl::OraQuery *& query, bool isQuery = false, int prefetch = 10);
        void exec( const std::string& statement, QueryAdapter*& pQueryAdapter, bool isQuery = false, int prefetch = 10);
		
		/**
		* fetchBLOB
		*
		* Fetches the BLOB data from the field/row specified by the previous
		* exec(). 
		*
		* @param	data	the BLOB data will be written to this parameter
		* @exception DBException Thrown if there is an OCI error.
		*
		*/
		void fetchBLOB(const std::string &strSQL, const std::string &colomnName, DataClobArray& data );
		
		
		/**
		* updateBLOB
		*
		* Updates the BLOB data from the field/row specified by the previous
		* exec(), with the contents of the vector. 
		*
		* @param	data	the BLOB data will be written from this parameter
		* @exception DBException Thrown if there is an OCI error.
		*
		*/
		void updateBLOB(const std::string &strSQL, 
			const std::string &colomnName,
			const std::vector<unsigned char>& data);

		/**
		* incrementExecutionCount
		*
		* Increment the count of threads that are using this connection right now.
		* Note: The count does not include threads that have this connection open, 
		* but are not executing on it right now.
		*/
		void incrementExecutionCount();
		
        
		/**
		* decrementExecutionCount
		*
		* Decrement the count of threads that are using this connection right now.
		* Note: The count does not include threads that have this connection open,
		* but are not executing on it right now.
		*/
		void decrementExecutionCount();
		
		/**
		* getExecutionCount
		*
		* Get the count of threads that are using this connection right now.
		* Note: The count does not include threads that have this connection open,
		* but are not executing on it right now.
		*/
		int getExecutionCount() { return m_executionCount; };
		
		/**
		* isConnectionInUse
		*
		* Checks if the connection is still being used.  This involves checking
		* if the execution count is non-zero or if there are any statements 
		* that have more data to be fetched.  Note: this only returns true
		* if there is a thread that is partway through a transaction on this
		* connection - not just if they have an open link to this connection.
		*
		* @return bool     True if the connection is being used at the moment.
		*                  False otherwise.
		*/
		bool isConnectionInUse();

		//static void cleanUpQuery(ocl::OraQuery*& Query);
        void cleanQuery( QueryAdapter*& pQuery );
		
protected:
	
	/**
	* createConnection
	* 
	* creates the underlying framework to support a session
	*/
    void createConnection(void);
	
	// Database connection data
	
	const std::string m_connectionStr;
	const std::string m_databaseName;
	const std::string m_userName;
	const std::string m_password;
	const std::string m_hostName;
	
	//TD18251, jianghp, to replace the OCI with OCL
	ocl::OraConnection *m_connection;
	//ocl::OraQuery      *m_currentQuery;
    // use map for faster find method
    std::map<QueryAdapter*, long> m_mapActiveQuery;
	// The number of SimpleDb objects (or other database access objects)
	// that are sharing this connection.
	unsigned long m_usageCount;
	
	// The count of threads that are using this connection right now.
	// The count does not include threads that have this connection open,
	// but are not executing on it right now (ie. m_usageCount)
	
	unsigned long m_executionCount;
	
	// Enables thread guarding
	TA_Base_Core::ReEntrantThreadLockable m_lock;
	TA_Base_Core::ReEntrantThreadLockable m_countLock;
	
//commented below, this solution does work.
	//unsigned long m_queryTimeout;
	//	ocl::OraTransaction* m_tran;
	
	private:
		// the lock for m_mapActiveQuery, because it will be accessed by multi thread
		TA_Base_Core::ReEntrantThreadLockable m_queryLock;
};

}
#endif // OCICONNECTION_H



