/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/scada_historian/scada_historian_data_access/src/OCIInsertAccess.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// OCIInsertAccess.h: interface for the OCIInsertAccess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCIACCESS_H__D1DFE01D_96B4_4758_8BA5_5B8E2829B53A__INCLUDED_)
#define AFX_OCIACCESS_H__D1DFE01D_96B4_4758_8BA5_5B8E2829B53A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "app/history/scada_historian/scada_historian_data_access/src/InsertDBAccess.h"
#include "core/database/src/BindVars.h"

// NOTE: This oci is the Oracle 'OCI.H'. Unfortunately there is an 'OCI.h' in
//			CORBA as well....
//       This oci.h needs to be included AFTER InsertDBAccess.h
#include <oci.h>


/* exit flags */
#define OCI_EXIT_FAILURE 1
#define OCI_EXIT_SUCCESS 0


namespace TA_App
{
    
        class OCIInsertAccess : public InsertDBAccess  
        {
        public:
	        OCIInsertAccess(const char* hostName, const char* databaseName,
                      const char* userName, const char* password);

	        virtual ~OCIInsertAccess();

	        virtual bool open();
	        virtual bool close();

            virtual bool exec(std::string command, long &retCode, 
							        int bindType = InsertDBAccess::BY_POSITION, BindVars *bind = 0, 
							        int commit = 1);

	        //virtual bool CreateTable();
	        //virtual bool DeleteTable();

	        //virtual bool get(DBAObject &key, DBAObject &data);
	        //virtual bool put(DBAObject &key, DBAObject &data);

	        virtual bool StatementCommit();
	        virtual bool StartTransaction();
	        virtual bool rollback();
	        virtual bool commit();

	        virtual bool EvaluateLastStatus(char * modName = __FILE__, int line = __LINE__);

	        virtual bool ServerName(char* result, int maxSize, int &resultLen);
	        virtual bool DriverName(char* result, int maxSize, int &resultLen);
	        virtual bool DriverVersion(char* result, int maxSize, int &resultLen);
	        virtual bool DatabaseName(char* result, int maxSize, int &resultLen);

	        // Attach the server and set the context
	        bool AttachServer();
	        // Detach from the server
	        bool DetachServer();
	        // Attempt to reconnect to the server
	        bool ReconnectServer();
	        // Re-attempt the execution of the statement (after reconnecting)
	        bool RetryStatement();
	        // Set the username and password for this session
	        bool SetUserNamePassword();
	        // Reset the DBConnection from the run params (if possible)
	        bool ResetDBConnection();
	        
        protected:
	        // Various handles we need to operate the Oracle OCI interface.
	        OCIEnv		*m_env;
	        OCIServer	*m_server;
	        OCISession	*m_session;
	        OCIError	*m_error;
	        OCISvcCtx	*m_context;
	        OCIStmt		*m_statement;

	        sb4			m_nativeCode;

	        char		m_errorMsg[130];
        };

  
} // TA_App

#endif // !defined(AFX_OCIACCESS_H__D1DFE01D_96B4_4758_8BA5_5B8E2829B53A__INCLUDED_)

