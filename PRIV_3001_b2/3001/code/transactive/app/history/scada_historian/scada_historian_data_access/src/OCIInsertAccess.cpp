/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/scada_historian/scada_historian_data_access/src/OCIInsertAccess.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// OCIInsertAccess.cpp: implementation of the OCIInsertAccess class.
//
//	Specific to ORACLE OCI interface.
//
//
//
//
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include "app/history/scada_historian/scada_historian_data_access/src/OCIInsertAccess.h"
#include "app/history/scada_historian/scada_historian_data_access/src/InsertDBError.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/database/src/SimpleDb.h"


using namespace TA_Core;


namespace TA_App
{
    
        //////////////////////////////////////////////////////////////////////
        // Construction/Destruction
        //////////////////////////////////////////////////////////////////////

        //-----------------------------------------------------------------------------
        //
        //
        OCIInsertAccess::OCIInsertAccess(const char* hostName, const char* databaseName,
                             const char* userName, const char* password)
	        : InsertDBAccess( hostName, databaseName, userName, password ),
	          m_env(0), 
	          m_server(0),
	          m_session(0),
	          m_error(0),
	          m_context(0),
	          m_statement(0),
	          m_nativeCode(0)
        {
	        // Create a suitable environment assuming we are doing multi-user
	        // and multi-session.

	        try
	        {
		        m_LastStatus = OCIEnvCreate(	&m_env, OCI_THREADED|OCI_OBJECT,
												        0,				// memory callback
												        0,				// memory allocation function
												        0,				// memory re-allocation
												        0,				// memory free function
												        0,				// size of extra mem for environment
												        0	);			// ptr to extra memory 


		        if (m_LastStatus != OCI_SUCCESS)
		        {
			        EvaluateLastStatus(__FILE__, __LINE__);
		        }

		        // ...and let the nightmare begin...
		        // First, we need a server handle
		        if ((m_LastStatus = OCIHandleAlloc(m_env, (void**)&m_server, OCI_HTYPE_SERVER, 0, 0) )
				        != OCI_SUCCESS)
		        {
			        EvaluateLastStatus(__FILE__, __LINE__);
		        }

		        // now we need an error handle
		        if ((m_LastStatus = OCIHandleAlloc(m_env, (void**)&m_error, OCI_HTYPE_ERROR, 0, 0) )
				        != OCI_SUCCESS)
		        {
			        EvaluateLastStatus(__FILE__, __LINE__);
		        }

		        // Next one would be a service context handle
		        if ((m_LastStatus = OCIHandleAlloc(	m_env, (void **)&m_context, OCI_HTYPE_SVCCTX,
														        0, 0 ) )
				        != OCI_SUCCESS)
		        {
			        EvaluateLastStatus(__FILE__, __LINE__);
		        }
  
		        // Attach the server and set the context
		        AttachServer();

		        // Need also a session handle
		        if ((m_LastStatus = OCIHandleAlloc(	m_env, (void**)&m_session, 
														        OCI_HTYPE_SESSION, 0, 0) )
				        != OCI_SUCCESS)
		        {
			        EvaluateLastStatus(__FILE__, __LINE__);
		        }

		        // Set the user name and password for this session.
		        SetUserNamePassword();

		        // For later we need a statement handle as well.
        //		if ((m_LastStatus = OCIHandleAlloc(	m_env, (void**)&m_statement, 
        //														OCI_HTYPE_STMT, 0, 0) )
        //				!= OCI_SUCCESS)
        //		{
        //			EvaluateLastStatus(__FILE__, __LINE__);
        //		}
	        }

	        catch(InsertDBError& )
	        {
		        printf( "Unrecoverable error. Check LogFile for reasons. "
			            "No connection to database.\n\n" );
	        }

	        // That'll be all for now. 'SessionOpen' will be handled during 'open'.
        }


        //-----------------------------------------------------------------------------
        //
        //
        OCIInsertAccess::~OCIInsertAccess()
        {
	        // Close the session first
	        if (isOpen() == true)
	        {
		        close();
	        }

	        // The thorough error checking is important only during development.
	        // In real life, if this module gets destroyed we don't worry about
	        // the handles anymore. If the cleanup fails there is nothing we can do.
        #ifdef DEBUG
	        try
	        {
		        // Detach from the server
		        DetachServer();

		        // Get rid of all those handles we have allocated.
		        if ((m_LastStatus = OCIHandleFree(m_statement, OCI_HTYPE_STMT) )
				        != OCI_SUCCESS)
		        {
			        EvaluateLastStatus(__FILE__, __LINE__);
		        }

		        if ((m_LastStatus = OCIHandleFree(m_server, OCI_HTYPE_SERVER) )
				        != OCI_SUCCESS)
		        {
			        EvaluateLastStatus(__FILE__, __LINE__);
		        }

		        if ((m_LastStatus = OCIHandleFree(m_context, OCI_HTYPE_SVCCTX) )
				        != OCI_SUCCESS)
		        {
			        EvaluateLastStatus(__FILE__, __LINE__);
		        }

		        if ((m_LastStatus = OCIHandleFree(m_session, OCI_HTYPE_SESSION) )
				        != OCI_SUCCESS)
		        {
			        EvaluateLastStatus(__FILE__, __LINE__);
		        }
	        }
	        catch(InsertDBError& e)
	        {
	        }
        #else
	        // if it fails it fails... nothing we can do.
	        OCIServerDetach(m_server,	m_error, OCI_DEFAULT );

	        OCIHandleFree(m_statement, OCI_HTYPE_STMT);
	        OCIHandleFree(m_server,		OCI_HTYPE_SERVER);
	        OCIHandleFree(m_context,	OCI_HTYPE_SVCCTX);
	        OCIHandleFree(m_session,	OCI_HTYPE_SESSION);
        #endif

	        // No use of checking on this one as this method needs the error handle.
	        OCIHandleFree(m_error, OCI_HTYPE_ERROR);
        }




        //-----------------------------------------------------------------------------
        //
        //
        bool OCIInsertAccess::ServerName(char *result, int maxSize, int &resultLen)
        {
	        printf("OCIInsertAccess::ServerName() not implemented yet.\n");

	        return (m_LastStatus != 0);							
        }


        //-----------------------------------------------------------------------------
        //
        //
        bool OCIInsertAccess::DriverName(char *result, int maxSize, int &resultLen)
        {
	        printf("OCIInsertAccess::DriverName() not implemented yet.\n");

	        return (m_LastStatus != 0);							
        }


        //-----------------------------------------------------------------------------
        //
        //
        bool OCIInsertAccess::DriverVersion(char *result, int maxSize, int &resultLen)
        {
	        printf("OCIInsertAccess::DriverVersion() not implemented yet.\n");

	        return (m_LastStatus != 0);							
        }


        //-----------------------------------------------------------------------------
        //
        //
        bool OCIInsertAccess::DatabaseName(char *result, int maxSize, int &resultLen)
        {
	        printf("OCIInsertAccess::DatabaseName() not implemented yet.\n");

	        return (m_LastStatus != 0);							
        }


        //-----------------------------------------------------------------------------
        //
        //
        bool OCIInsertAccess::EvaluateLastStatus(char * modName, int line)
        {
	        switch (m_LastStatus)
	        {
	        case OCI_INVALID_HANDLE:
		        printf( "Invalid Handle passed. "
				        "No further information available.\n\n" );
		        return true;

	        case OCI_NEED_DATA:
                printf("OCI_NEED_DATA: Need run-time data.\n\n" );
		        return true;

	        case OCI_NO_DATA:			// No data is not an error.
	        case OCI_SUCCESS:
		        return true;

	        case OCI_STILL_EXECUTING:
		        printf("OCI_STILL_EXECUTING.\n");
		        break;

	        case OCI_CONTINUE:
		        printf("OCI_CONTINUE.\n");
		        break;

	        case OCI_SUCCESS_WITH_INFO:
		        printf("OCI_SUCCESS_WITH_INFO.\n");
		        break;

	        case OCI_ERROR:
		        printf("OCI_ERROR.\n");
		        break;

	        default:
		        break;
	        }

	        InsertDBAccess::EvaluateLastStatus(modName, line);

	        // Have a closer look to the error. Retrieve more info from Oracle.
	        memset(m_errorMsg, 0, sizeof(m_errorMsg));
	        m_nativeCode = 0;

	        OCIErrorGet(m_error, 1, 0, &m_nativeCode, (unsigned char *) &m_errorMsg, 
								        sizeof(m_errorMsg), OCI_HTYPE_ERROR );

	        // NativeCode == 0 means there is no error. Don't print out anything then.
	        if (m_nativeCode != 0)
	        {
		        printf("ORACLE OCI error: %s, NativeCode: %d\n", m_errorMsg, m_nativeCode);

		        //DBErrorPtr err = new InsertDBError;
                InsertDBError err;

		        err.module(modName);
		        err.line(line);
		        err.status(m_nativeCode);
		        err.reason(m_errorMsg);

		        throw( err );
	        }

	        // This method never fails.
	        return true;
        }


        //-----------------------------------------------------------------------------
        //
        //
        bool OCIInsertAccess::close()
        {
	        try
	        {
		        if ((m_LastStatus = OCISessionEnd(	m_context, m_error, m_session,
														        OCI_DEFAULT) )
				        != OCI_SUCCESS )
		        {
			        EvaluateLastStatus(__FILE__, __LINE__);
		        }

		        m_OpenStatus = false;
	        }
	        catch(InsertDBError& )
	        {
	        }

	        return (m_LastStatus == OCI_SUCCESS);
        }


        //-----------------------------------------------------------------------------
        //
        //
        bool OCIInsertAccess::open()
        {
            LOG(SourceInfo, DebugUtil::FunctionEntry, "open");

	        try
	        {
		        if ((m_LastStatus = OCISessionBegin(m_context, m_error, m_session,
														        OCI_CRED_RDBMS, OCI_DEFAULT) )
				        != OCI_SUCCESS )
		        {
			        EvaluateLastStatus(__FILE__, __LINE__);
                    return false;
		        }

		        // Connect our context with this user session.
		        if ((m_LastStatus = OCIAttrSet(	m_context, OCI_HTYPE_SVCCTX, 
													        m_session, 0, OCI_ATTR_SESSION, m_error ) )
				        != OCI_SUCCESS )
		        {
			        EvaluateLastStatus(__FILE__, __LINE__);
		        }

		        // If we made it till here we have a session going.
		        m_OpenStatus = true;
	        }

	        catch(InsertDBError& )
	        {
	        }

            LOG(SourceInfo, DebugUtil::FunctionExit, "open");

	        return (m_LastStatus == OCI_SUCCESS);
        }



        //-----------------------------------------------------------------------------
        //
        //
        bool OCIInsertAccess::StartTransaction()
        {
        /*	// Need a transaction handle

	        // Start a new transaction. End either with commit() or rollback().
	        if ((m_LastStatus = OCITransStart(m_context, m_error, 60, OCI_TRANS_NEW) ) 
			        != OCI_SUCCESS )
	        {
		        EvaluateLastStatus(__FILE__, __LINE__);
	        }
        */
	        return false;
        }


        //-----------------------------------------------------------------------------
        //
        // This function is for TRANSACTION commit's.
        // Not to be used for a simple commit after performing some SQL commands.
        //
        bool OCIInsertAccess::commit()
        {
	        try
	        {
        //		if ((m_LastStatus = OCITransCommit(...) )
        //				!= OCI_SUCCESS )
        //		{
        //			EvaluateLastStatus(__FILE__, __LINE__);
        //		}
	        }

	        catch(InsertDBError& )
	        {
	        }

	        return false;
        }


        //-----------------------------------------------------------------------------
        //
        //
        bool OCIInsertAccess::rollback()
        {
	        // OCITransRollback(...)
	        return false;
        }


        //-----------------------------------------------------------------------------
        //
        //
        /*bool OCIInsertAccess::get(DBAObject &key, DBAObject &data)
        {

	        return false;
        }*/


        //-----------------------------------------------------------------------------
        //
        //
        /*bool OCIInsertAccess::put(DBAObject &key, DBAObject &data)
        {
	        // Tablename has to be known.
	        if (m_TableName.length() == 0)
	        {
		        return false;
	        }

            std::string statement = "INSERT  ";

	        statement +=	m_TableName;
	        statement +=	" (startdate DATE, enddate DATE, cost NUMBER); ";

	        long	rc;
	        exec( statement.c_str(), rc );


	        return false;
        }*/


        //-----------------------------------------------------------------------------
        //
        //
        /*bool OCIInsertAccess::CreateTable()
        {
	        // Tablename has to be known.
	        if (m_TableName.length() == 0)
	        {
		        return false;
	        }

            std::string statement = "CREATE TABLE ";

	        statement +=	m_TableName;
	        statement +=	" (startdate DATE, enddate DATE, cost NUMBER); ";

	        long	rc;
	        exec( statement.c_str(), rc );

	        //OCIBindObject
	        //OCIBindByPos
	        //OCIStmtFetch

	        return (m_LastStatus == OCI_SUCCESS);
        }*/


        //-----------------------------------------------------------------------------
        //
        //
        /*bool OCIInsertAccess::DeleteTable()
        {
            std::string statement = "DROP TABLE " ;

	        statement +=	m_TableName;
	        statement +=	" with contents; "; 


	        return (m_LastStatus == OCI_SUCCESS);
        }*/


        //-----------------------------------------------------------------------------
        //
        //
        bool OCIInsertAccess::exec(std::string command, long &retCode, 
							        int bindType, BindVars *bindVector, 
							        int commit)
        {
	        int mode = OCI_DEFAULT;
	        int attempts = 0;

	        if (commit != 0)
	        {
		        mode |= OCI_COMMIT_ON_SUCCESS;
	        }

	        do
	        {
		        try
		        {
			        if (m_statement != 0)
			        {
				        OCIHandleFree(m_statement, OCI_HTYPE_STMT);
			        }

			        if ((m_LastStatus = OCIHandleAlloc(	m_env, (void**)&m_statement, 
															        OCI_HTYPE_STMT, 0, 0) )
					        != OCI_SUCCESS)
			        {
				        EvaluateLastStatus(__FILE__, __LINE__);
			        }

			        // First prepare the statement.
			        if ((m_LastStatus = OCIStmtPrepare( m_statement, m_error, 
															        (unsigned char *) command.c_str(), 
															        command.length(),
															        OCI_NTV_SYNTAX, mode ) )
					        != OCI_SUCCESS )
			        {
				        EvaluateLastStatus(__FILE__, __LINE__);
			        }

			        // Bind variable names to database values if requested.
			        if ( bindVector != 0 )
			        {
				        bindVector->begin();

				        int		pos	= 1;
				        BindVar	*var	= 0;
				        while ((var = bindVector->next()) != 0)
				        {
					        // Translate the generic variable type	into something OCI specific.
					        int theType;
					        switch(var->type())
					        {
					        case DBA_STRING:			theType = SQLT_STR; break;
					        //case DBA_STRING:			theType = SQLT_CHR; break;
					        case DBA_INT:				theType = SQLT_INT; break;
					        case DBA_NUM:				theType = SQLT_NUM; break;

					        case DBA_DATE:				theType = SQLT_DATE; break;
					        case DBA_TIME:				theType = SQLT_TIME; break;
					        case DBA_TIME_TZ:			theType = SQLT_TIME_TZ; break;
					        case DBA_TIMESTAMP:		theType = SQLT_TIMESTAMP; break;
					        case DBA_TIMESTAMP_TZ:	theType = SQLT_TIMESTAMP_TZ; break;

					        default:
						        // Whatever datatype it is, it's not supported. Force an OCI error.
						        theType = -1;
					        }

					        // The bindPtr MUST be null on each Oracle bind call.
					        // It's never used since the bind handle will be released implicit.
					        OCIBind		*bindPtr		= 0;
					        OCIDefine	*DefinePtr	= 0;

					        switch (bindType)
					        {
					        case BY_DEFINE_POS:
						        m_LastStatus = 
							        OCIDefineByPos ( m_statement, 
												          &DefinePtr,
												          m_error,
												          pos++,
												          const_cast<void *>(var->addr()),
												          var->len(),
												          theType,
												          0,
												          0,
												          0,
												          OCI_DEFAULT );
						        break;

					        case BY_NAME:
						        m_LastStatus = 
							        OCIBindByName(	m_statement, 
												        &bindPtr,
												        m_error,
												        (unsigned char *) const_cast<char *>(var->name()),
												        -1,
												        const_cast<void *>(var->addr()),
												        var->len(),
												        theType,
												        0,
												        0,
												        0,
												        0,
												        0, 
												        OCI_DEFAULT  );

					        case BY_POSITION:
						        m_LastStatus = 
							        OCIBindByPos(	m_statement, 
												        &bindPtr,
												        m_error,
												        pos++,
												        const_cast<void *>(var->addr()),
												        var->len(),
												        theType,
												        0,
												        0,
												        0,
												        0,
												        0, 
												        OCI_DEFAULT  );
					        default:
						        break;
					        }
					        
					        if (m_LastStatus != OCI_SUCCESS )
					        {
						        EvaluateLastStatus(__FILE__, __LINE__);
						        break;
					        }
				        }
			        }

			        // At last execute the statement.
			        if ((m_LastStatus = OCIStmtExecute( m_context, m_statement, m_error, 
															        1, 0, 0, 0, OCI_DEFAULT ) ) 
					        != OCI_SUCCESS )
			        {
				        EvaluateLastStatus(__FILE__, __LINE__);
			        }

			        // The flag 'OCI_COMMIT_ON_SUCCESS' doesn't seem to work...
			        // Do it manually.
			        if (commit != 0)
			        {
				        StatementCommit();
			        }
		        }

		        catch(InsertDBError& e)
		        {
			        // Within this method, this error should only occur during the 
			        // OCIStmtExecute.
			        if( 3113 == e.status() )
			        {
				        if( RetryStatement() == false )
					        break;
				        // Force the retry
				        m_LastStatus = OCI_ERROR;
			        }
			        else
			        {
				        break;
			        }
		        }

		        // Increment the number of events
		        attempts++;
	        } while ( m_LastStatus != OCI_SUCCESS && attempts <= 2 );

	        retCode = m_LastStatus;
	        return (m_LastStatus == OCI_SUCCESS);
        }


        //-----------------------------------------------------------------------------
        //
        //
        bool OCIInsertAccess::StatementCommit()
        {
	        // We don't want to call on strlen() every time we get here. Therefore
	        // lets use an array of chars. We use the sizeof() operator to determine 
	        // the string length at compile time.
	        static char commit[]	= { 'C','O','M','M','I','T' };

	        if ((m_LastStatus = OCIStmtPrepare( m_statement, m_error, 
													        (unsigned char *) commit, 
													        sizeof(commit),
													        OCI_NTV_SYNTAX, OCI_DEFAULT ) )
			        != OCI_SUCCESS )
	        {
		        EvaluateLastStatus(__FILE__, __LINE__);
	        }

	        if ((m_LastStatus = OCIStmtExecute( m_context, m_statement, m_error, 
													        1, 0, 0, 0, OCI_DEFAULT ) ) 
			        != OCI_SUCCESS )
	        {
		        EvaluateLastStatus(__FILE__, __LINE__);
	        }

	        return (m_LastStatus == OCI_SUCCESS);
        }

        // Attach the server and set the context
        bool OCIInsertAccess::AttachServer()
        {
	        // Create an access path to the data source
	        if ((m_LastStatus = OCIServerAttach( m_server, m_error, (unsigned char *)m_Databasename.c_str(), 
										         m_Databasename.size(), OCI_DEFAULT) ) != OCI_SUCCESS )
	        {
		        EvaluateLastStatus(__FILE__, __LINE__);
		        return false;
	        }

	        // Set the server context into the service context
	        if ((m_LastStatus = OCIAttrSet(	m_context, OCI_HTYPE_SVCCTX, m_server,
									        (ub4) 0, OCI_ATTR_SERVER, m_error) ) != OCI_SUCCESS )
	        {
		        EvaluateLastStatus(__FILE__, __LINE__);
		        return false;
	        }

	        return true;
        }

        // Detach from the server
        bool OCIInsertAccess::DetachServer()
        {
	        // Detach from the server
	        if ((m_LastStatus = OCIServerDetach (m_server, m_error, OCI_DEFAULT ) )
			        != OCI_SUCCESS)
	        {
		        EvaluateLastStatus(__FILE__, __LINE__);
		        return false;
	        }
	        return true;
        }

        // Attempt to reconnect to the server
        bool OCIInsertAccess::ReconnectServer()
        {
	        bool wasOpen = isOpen();

	        // End the oracle session
	        if (wasOpen == true)
	        {
		        close();
	        }

	        // Detatch the server connection
	        DetachServer();

	        // Reset the DBConnection from the run params (if possible)
	        try {
		        // Don't bother with the return code as we will attempt to reconnect
		        // with the old dbconnection if there is a problem with the new one.
		        ResetDBConnection();
	        }
	        catch( InsertDBError e )
	        {
		        return false;
	        }

	        
	        // Attach the server connection
	        if( AttachServer() == false )
	        {
		        return false;
	        }

	        // Start the oracle session (if it was open before)
	        if( wasOpen && open() == false )
	        {
		        return false;
	        }

	        return true;
        }

        // Set the username and password for this session
        bool OCIInsertAccess::SetUserNamePassword()
        {
	        try
	        {
		        // Connect username with this session.
		        if ((m_LastStatus = OCIAttrSet(m_session, OCI_HTYPE_SESSION,
									           const_cast<char*>(m_Username.c_str()),
									           strlen(m_Username.c_str()), OCI_ATTR_USERNAME, m_error) )
				        != OCI_SUCCESS)
		        {
			        EvaluateLastStatus(__FILE__, __LINE__);
			        return false;
		        }

		        // ...and make the password known to this session as well.
		        if ((m_LastStatus = OCIAttrSet(m_session, OCI_HTYPE_SESSION,
									           const_cast<char*>(m_Password.c_str()),
									           strlen(m_Password.c_str()), OCI_ATTR_PASSWORD, m_error) )
				        != OCI_SUCCESS)
		        {
			        EvaluateLastStatus(__FILE__, __LINE__);
			        return false;
		        }
	        }

	        catch(InsertDBError& )
	        {
		        // Output a debug warning
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "OCIInsertAccess: Unable to retrieve new DBConnection");
		        // Rethrow the error to be handled by calling code
		        throw;
	        }

	        return true;
        }

        // Reset the DBConnection from the run params (if possible)
        bool OCIInsertAccess::ResetDBConnection()
        {
            // Get the database connection name, username and password
            std::string connect = RunParams::getInstance().get(RPARAM_DBCONNECTION);

	        // If there is no connection returned, forget it as it may not be a managed
	        // process using RunParams.
	        if( connect.size() == 0 )
	        {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "OCIInsertAccess: Unable to retrieve new DBConnection");
		        return false;
	        }

            // Parse temp to get the database info
            std::vector<std::string> dbData;
            SimpleDb::ParseConnectStr(connect, dbData);
            if (dbData.size() < 3)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "OCIInsertAccess: DBConnection retrieve is not valid");
		        return false;
	        }

	        // Check if the database connection has changed
	        if( m_Databasename != dbData[0].c_str() )
	        {
		        // NOTE: The attach server code will reset the connection with the
		        // new database connection.
		        m_Databasename = dbData[0].c_str();
	        }

	        // It is unlikely that the username and password would change but
	        // the following code handles this.
	        if( m_Username != dbData[1].c_str() ||
		        m_Password != dbData[2].c_str() )
	        {
		        // Set to the new values
		        m_Username = dbData[1].c_str();
		        m_Password = dbData[2].c_str();

		        // Set the username and password
		        if( SetUserNamePassword() == false )
		        {
			        return false;
		        }
	        }

	        return true;
        }


        // Re-attempt the execution of the statement (after reconnecting)
        bool OCIInsertAccess::RetryStatement()
        {
	        try {
		        // Attempt to reconnect the server
		        if( ReconnectServer() == false )
			        return false;
	        }
	        catch( InsertDBError& )
	        {
		        // Output a debug warning but give up
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "OCIInsertAccess: Unable to retry statement");
		        return false;
	        }

	        // Success
	        return true;
        }

    
} // TA_App

