/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/database_status_monitor/src/SimpleDbStatusChecker.h $
 * @author:  J.Chevalley
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 */

#if !defined(SimpleDBStatusChecker_85CF6411_35CE_42cb_8BE1_5FE8FDEB40C7__INCLUDED_)
#define SimpleDBStatusChecker_85CF6411_35CE_42cb_8BE1_5FE8FDEB40C7__INCLUDED_

#include <string>
//
#include "core/threads/src/Thread.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "bus/database_status_monitor/DsmCorbaDef/IDL/src/DatabaseStatusCorbaDef.h"
#include "boost/shared_ptr.hpp"

namespace ocl
{
	class OraConnection;
	class OraQuery;
}


namespace TA_Base_Core
{
	class SingletonTimerUtil;
}

namespace TA_Base_Bus
{
	class IDatabaseStatusObserver;
    class SimpleDbStatusChecker : public TA_Base_Core::Thread,
								  public virtual TA_Base_Core::ITimeoutCallback
    {

    public:
	    /**
	     * Constructor.
	     * @param connectionInfo    A connection string in the form "DATABASE:USER:PWD"
	     * @param periodInSeconds    The database status check period in seconds.
	     * @param observer    A reference to the status observer.
	     * 
	     */
	    SimpleDbStatusChecker(const std::string& connectionInfo, unsigned long periodInSeconds, IDatabaseStatusObserver *observer);
        virtual ~SimpleDbStatusChecker();

    public:
	    /**
	     * The thread's main loop.
	     */
	    virtual void run();
	    virtual void terminate();

		//Implement for ITimeoutCallback;
		void timerExpired(long timerId, void* userData);

		void setDatabaseOffLine();

    private:
	    /**
	     * Updates the stored database status.
	     * @param status
	     * 
	     */
        void setDatabaseStatus(DatabaseStatusCorbaDef::EState status);
        bool Connect();
        void Poll();
		void Init();


    private:
        DatabaseStatusCorbaDef::EState m_dbStatus;
        std::string m_dbConnectionInfo;
        std::string m_hostName;
        std::string m_schemaName;
        std::string m_dbPassword;
		ocl::OraConnection * m_connection;
	    IDatabaseStatusObserver *m_dbStatusObserver;
	    unsigned long m_checkPeriodInSeconds;
        bool m_run;

		// Wenguang++
		TA_Base_Core::SingletonTimerUtil& m_timerUtility;
		ocl::OraQuery *m_pQuery;
		bool m_reConnect;
    };
} //namespace
#endif // !defined(SimpleDBStatusChecker_85CF6411_35CE_42cb_8BE1_5FE8FDEB40C7__INCLUDED_)

