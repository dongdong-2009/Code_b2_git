#ifndef DBSYNCH_POINT_H__
#define DBSYNCH_POINT_H__

#include "app/system_control/system_status_agent/src/taComponentsDriver/ISelfResolvingPointContainer.h"

namespace TA_Base_Core
{
	class SimpleDb;
}

namespace ocl
{
    class OraConnection;
    class OraQuery;
}

namespace sql
{
	class Driver;
	class Connection; 
	class ResultSet;
}

namespace TA_Base_Bus
{
	class EnumDataPoint;
}

namespace TA_Base_App
	{
	class DbSynchPoint : public ISelfResolvingPointContainer 
	{
	public:

		/**
		 * DbServerPoint	
		 * 
		 * DTOR
		 *
		 */
		virtual ~DbSynchPoint();

		/**
		 * getDataPoint	
		 * 
		 * @return the datapoint owned by this point
		 *
		 */
		virtual TA_Base_Bus::DataPoint* const getDataPoint();

		/*
		* getAddressIdentifier
		*
		* @return the raw IP address or hostname of the point.
		* Note that the address configured for the datapoint will probably contain more than
		* the above, e.g. mftStatus:192.168.70.60. This metod should just return 192.168.70.60 bit
		*/
		virtual std::string getAddressIdentifier();

		/**
		 * onInit
		 * 
		 * This is called for the first and only time when the mode changes
		 * from INIT to something else (either monitor or control) 
		 * on the driver that owns this point. Hence, you should do any
		 * value/state/alarm manipulation required here. Note that when the
		 * agent starts the point will already have a quality of:
		 * TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED
		 *
		 * @param pEntityData contains the updated configuration data
		 *
		 * This implimentation attempts to resolve and store a pointer
		 * to the System Controller. It does not throw is this can not be 
		 * accomplished
		 * It also sets the corba time out and port number via run params.
		 */
		virtual void onInit(TA_Base_Core::SystemStatusAgentEntityDataPtr pEntityData);

		/**
		 * 
		 * onlineConfigUpdate
		 *
		 * This method is called when onlineConfigUpdate is called on the TaComponentsDriver. 
		 * You should process any parameters specific to your point.
		 *
		 * This method will be called when the agent configuration is changed
		 * online. Note that online datapoint configuration updates are
		 * not passed to driver.
		 *
		 * This method updates the timeout parameter
		 *
		 * @param changedParams contains the names of the params that have changed
		 *
		 * See DB_SERVER_TIMEOUT_PARAM comments at top of class
		 */
		virtual void onlineConfigUpdate(std::vector<std::string> changedParams);

	   /**
		* convertToIPointContainer
		*
		* If this ISelfResolvingPointContainer can keep this dataPoint updated it should return
		* a ISelfResolvingPointContainer* which contains this point. If the ISelfResolvingPointContainer
		* does not want this point return 0
		* The IPointContainer* should be created on the heap. You do NOT need to
		* delete this point on destruction
		*/
		static ISelfResolvingPointContainer* convertToIPointContainer(TA_Base_Bus::DataPoint* const dataPoint);


		/**
		  *
		  * updateDataPoint
		  *
		  * This method first calls updateDataPointBasedOnPing()
		  * to see if it should bother trying to update the point. If the ping is OK it then
		  * employs derived class methods to update the data point
		  *
		  * This method also ensures that isUpdating() returns true
		  * when this method is running - not thread-safe.
		  */
		virtual void updateDataPoint();

		 /**
		* isUpdating
		*
		* returns false if the point is currently processing a call
		* to updateDataPoint().
		* THIS METHOD IS NOT THREAD-SAFE! Hence it is assumed that a
		* single client thread is employing this method
		*/
		virtual bool isUpdating();

	protected:
	   /**
		* updatePointBasedOnProcessStatus
		*
		* update the state of the point based on the status of the related object. 
		* You should set an appropriate state if even you can not read the status of the
		* related process (but return false). Ignore the posibliltiy of Lan failure, e.g. if you could
		* not reach a Data Base Server process set low, GOOD quality
		*
		*@precondition: isResolved() returns true
		* 
		*@return false if you can not resolve the process/object required to read
		* the status. If you update the point successfully return true.
		*
		* @throw if you can not write to the datapoint
		*
		*/
		virtual bool updatePointBasedOnProcessStatus();

	   /*
		* isResolved
		*
		*@return true if you feel that resolve() should not need not be called
		* prior to calling updateBasedOnProcessStatus(). Note that you can return true
		* even if it is possible (but unlikely) that the resolved object is no longer
		* valid
		* 
		*/
		virtual bool isResolved();	  

	   /**
		* resolve
		*
		* resolve or re-resove (and store) the process or object that is used to 
		* determine the state that the datapoint should be set to
		*
		*@return true if the resolution is OK - else false
		* 
		* This implimentation creates and opens a database instance
		*
		* Note that this method connects to the database with a connection paramaters
		* that are specific to a given location rather than using the connection 
		* string passed in by the framework. That is because the one passed in 
		* by the framework may well point to the OCC server, rather than the server
		* we are tying to resolve - see resolve() for more info.
		*
		* The database connection string used has the form:
		* : <m_hostName>:<m_schemaName>:<m_dbPassword>
		* where m_hostName is supplied by datapoint config,
		* and m_schemaName and m_dbPassword are (hidden) run-time params.
		*
		* Note that the tnsnames.ora file must be configured so that 
		* the alias in that file is identical to the hostname for the
		* machine that the server instance is running on - IP addresses
		* can not be used. This convention is used so that the address config
		* can be used to both identfy the box that the db server is running on
		* and the alias in the tnsnames.ora file
		*/
		virtual bool resolve();	
		
	private:

        //TA_Base_Core::SimpleDbStatusMonitor* m_dataBase;
        //Bohong++
        ocl::OraConnection* m_pOraConnection;
        ocl::OraQuery*      m_pOraQuery;
		sql::Driver*        m_pMysqlDriver;
		sql::Connection*    m_pMysqlConntion;
		sql::ResultSet*     m_pMysqlQuery;
        //++Bohong

		TA_Base_Bus::EnumDataPoint* const m_datapoint;
	   
	   /**		
		* The database connection string used has the form:
		* : <m_hostName>:<m_schemaName>:<m_dbPassword>
		* where m_hostName is supplied by datapoint config,
		* and m_schemaName and m_dbPassword are (hidden) run-time params.
		*
		* Note that the tnsnames.ora file must be configured so that 
		* the alias in that file is identical to the hostname for the
		* machine that the server instance is running on - IP addresses
		* can not be used. This convention is used so that the address config
		* can be used to both identfy the box that the db server is running on
		* and the alias in the tnsnames.ora file
		*/
		std::string m_strHostName;   // for MySQL DB server or IP address
		std::string m_strServerName; // for Oracle TNS server name
		std::string m_strUserName;
		std::string m_strPassword;
		std::string m_strShema;     // for Mysql DB server name

		TA_Base_Core::SystemStatusAgentEntityDataPtr m_entityData;

		//Database timeout param set via run param
		int m_dbTimeOutSec;
		int m_nDBType;
		bool m_isUpdating;

		static std::vector<std::string> m_dbStatusNames;

		static std::string s_strDbSynOff;
		static std::string s_strDbSynOn;
		static std::string s_strDbSynFailure;
		static std::string s_strDbSynUnknow;

		enum EDbSynState
		{
			E_OFF     = 0,  
			E_ON,            
			E_FAILURE,
			E_UNKNOW
		};

	private:
		//private methods:

        /** 
          * DbServerPoint
          *
          * @param DataPoint* const datapoint is the point
		  * to update. Precondition the point is a registered CORBA object  
		  *
		  * @param const std::string hostName is the hostname where the CS is running
          */
		DbSynchPoint( 
			TA_Base_Bus::EnumDataPoint* const datapoint,
			const std::string& hostName
		);


        /** 
          * poll
          *
		  * This call will poll the db server - to determine if it is happy
		  *
          * @throw if the db Server does not communicate, i.e. not happy!
          */
		void poll(int& nNewValue);

        /** 
          * getConnectionParameters
          *
		  * retrieves the database user name and password from the connection
		  * string supplied by the framework
          */
		void getConnectionParameters( std::string& username, std::string& password );

        //Bohong++
        bool _Connect();
        void _DetachDb();

		// oyzl++
		void _Init();
		void _OraConnection();
		void _MysqlConnect();
		void _DoOraQuery(int& nNewValue);
		void _DoMysqlQuery(int& nNewValue);
		void _ParseDBType(const std::string& strDBName);
		int  _GetDbSynStatue();
		std::string _ConvertStatus2Str(int nNewValue);

        bool m_reConnect;
        //++Bohong
	};
} //end namespace TA_Base_App

#endif // DBSYNCH_POINT_H__
