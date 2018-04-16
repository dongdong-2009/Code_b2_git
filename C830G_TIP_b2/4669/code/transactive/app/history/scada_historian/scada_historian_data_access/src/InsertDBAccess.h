/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/scada_historian/scada_historian_data_access/src/InsertDBAccess.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// InsertDBAccess.h: interface for the InsertDBAccess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBACCESS_H__4521B230_F491_4B2D_BEEC_42CABBA8DD51__INCLUDED_)
#define AFX_DBACCESS_H__4521B230_F491_4B2D_BEEC_42CABBA8DD51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
//#include <windows.h>
#endif // _MSC_VER > 1000

//#include <DBAObject.h>
#include <string> 
#include "core/database/src/BindVars.h"


namespace TA_App
{
    
        class InsertDBAccess  
        {
        public:
	        enum bindType_e
	        {
		        BY_NAME	= 1,
		        BY_POSITION,
		        BY_DEFINE_POS
	        } DBBindTypes;

	        enum statusCodes_e
	        {
		        DBA_FAIL				= -1,
		        DBA_SUCCESS			= 0,
		        DBA_NO_DATA
	        } DBStatusCodes;

	        enum fieldTypes_e
	        {
		        DBA_STRING	= 1,
		        DBA_INT,
		        DBA_NUM,

		        DBA_DATE,
		        DBA_TIME,
		        DBA_TIME_TZ,
		        DBA_TIMESTAMP,
		        DBA_TIMESTAMP_TZ
	        } DBAFieldTypes;

	        virtual ~InsertDBAccess();

	        virtual bool open() = 0;
	        //virtual bool close() = 0;

            bool close();

	        //virtual bool put(DBAObject &key, DBAObject &data) = 0;
	        //virtual bool get(DBAObject &key, DBAObject &data) = 0;

	        virtual bool exec(std::string command, long &retCode, 
							        int bindType = BY_POSITION, BindVars *bind = 0, 
							        int commit = 1) = 0;

	        //virtual bool DeleteTable() = 0;
	        //virtual bool CreateTable() = 0;

	        virtual bool ServerName(char* result, int maxSize, int &resultLen) = 0;
	        virtual bool DriverName(char* result, int maxSize, int &resultLen) = 0;
	        virtual bool DriverVersion(char* result, int maxSize, int &resultLen) = 0;
	        virtual bool DatabaseName(char* result, int maxSize, int &resultLen) = 0;

	        virtual long	LastStatus();
	        virtual bool	isOpen();
	        virtual std::string TableName();
	        virtual bool	TableName(const char * name);
	        virtual bool	bind(	const char *name, const void *addr, int tokenType, 
								        int fieldLen, BindVars &bind);

	        virtual bool	EvaluateLastStatus(char * modName = __FILE__, int line = __LINE__);

        protected:
	        long	m_OpenStatus;
	        long	m_LastStatus;
	        std::string	m_Hostname;
	        std::string	m_Databasename;
	        std::string	m_Username;
	        std::string	m_Password;
	        std::string	m_TableName;

	        InsertDBAccess(const char* hostName, const char* databaseName,
                     const char* userName, const char* password);

        private:
        };

    
} // TA_App

#endif // !defined(AFX_DBACCESS_H__4521B230_F491_4B2D_BEEC_42CABBA8DD51__INCLUDED_)

