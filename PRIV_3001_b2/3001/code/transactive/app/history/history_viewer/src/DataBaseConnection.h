#if !defined(AFX_DATABASECONNECTION_H__B0856383_8CC5_11D6_B24E_0050BAB25370__INCLUDED_)
#define AFX_DATABASECONNECTION_H__B0856383_8CC5_11D6_B24E_0050BAB25370__INCLUDED_

/**
  * DataBaseConnection.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/history_viewer/src/DataBaseConnection.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class loads creates a database connection for use throughout
  * the program.  This class is a singleton.
  * NOTE: This class uses Oracle OCCI which is new to Oracle 9i.  To
  * run and compile, it is necessary to have the Oracle 9i client.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef USE_OCCI
#include <occi.h>
#else
#include "core/database/src/SimpleDB.h"
#endif // USE_OCCI


namespace TA_App
{

#ifdef USE_OCCI

    //
    // OCCI version of DataBaseConnection
    //

    class DataBaseConnection  
    {
    public:
        
        /**
		  * getInstance
		  * 
		  * Returns the one and only instance of DataBaseConnection.
		  *
		  */
        static DataBaseConnection& getInstance()
        {
	        static DataBaseConnection e;
	        return e;
        }

        /**
		  * ~DataBaseConnection
		  * 
		  * Destructor.
		  *
		  */
        ~DataBaseConnection();

        /**
		  * CreateConnection
		  * 
		  * This gets the connection string from RunParams and creates an OCCI
          * connection to the database..
		  *
          * @exception CouldNotConnectException - thrown if it cannot get 
          * connection parameters or connect.
		  */
        void CreateConnection();

        class CouldNotConnectException{};

        /**
		  * GetConnection
		  * 
		  * Returns the database connection.
		  *
          * @exception CouldNotConnectException - thrown if it cannot get 
          * connection parameters or connect.
		  */
        oracle::occi::Connection* GetConnection();

    private:
        
        DataBaseConnection() {m_Connected = false;}
        DataBaseConnection(DataBaseConnection&) {}
        DataBaseConnection& operator=(const DataBaseConnection &) {}

    private:

        bool m_Connected;
        oracle::occi::Environment *env;
        oracle::occi::Connection *conn;
    };

#else  // USE_OCCI

    //
    // SimpleDB version of DataBaseConnection
    //

    class SimpleDBConnection  
    {
    public:
        
        /**
		  * getInstance
		  * 
		  * Returns the one and only instance of SimpleDBConnection.
		  *
		  */
        static SimpleDBConnection& getInstance()
        {
	        static SimpleDBConnection e;
	        return e;
        }

        /**
		  * ~SimpleDBConnection
		  * 
		  * Destructor.
		  *
		  */
        ~SimpleDBConnection();

        /**
		  * CreateConnection
		  * 
		  * This gets the connection string from RunParams and creates an SimpleDB
          * connection to the database..
		  *
          * @exception CouldNotConnectException - thrown if it cannot get 
          * connection parameters or connect.
		  */
        void CreateConnection();

        class CouldNotConnectException{};

        /**
		  * GetConnection
		  * 
		  * Returns the database connection.
		  *
          * @exception CouldNotConnectException - thrown if it cannot get 
          * connection parameters or connect.
		  */
        SimpleDb* GetConnection();

        /**
		  * ClearConnection
		  * 
		  * Delete the connection
		  *
          */
        void ClearConnection();

    private:
        
        SimpleDBConnection() {m_Connected = false; m_SimpleDB=NULL;}
        SimpleDBConnection(SimpleDBConnection&) {}
        SimpleDBConnection& operator=(const SimpleDBConnection &) {}

    private:

        bool m_Connected;
        SimpleDb* m_SimpleDB;
    };
}

#endif // USE_OCCI
#endif // !defined(AFX_DATABASECONNECTION_H__B0856383_8CC5_11D6_B24E_0050BAB25370__INCLUDED_)





