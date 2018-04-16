/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/scada_historian/scada_historian_data_access/src/InsertDBAccess.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// InsertDBAccess.cpp: implementation of the InsertDBAccess class.
//
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <stdio.h>
#include "app/history/scada_historian/scada_historian_data_access/src/InsertDBAccess.h"


namespace TA_App
{
    
        //////////////////////////////////////////////////////////////////////
        // Construction/Destruction
        //////////////////////////////////////////////////////////////////////

        InsertDBAccess::InsertDBAccess(const char* hostName, const char* databaseName,
                           const char* userName, const char* password)
	        : m_OpenStatus(0), 
	          m_LastStatus(0),
              m_Hostname(hostName),
              m_Databasename(databaseName),
	          m_Username(userName),
	          m_Password(password)
        {

        }


        //-----------------------------------------------------------------------------
        //
        //
        InsertDBAccess::~InsertDBAccess()
        {
	        if (isOpen() == true)
	        {	
		        close();
	        }
        }

        //-----------------------------------------------------------------------------
        //
        //
        bool InsertDBAccess::isOpen()
        {
	        return (m_OpenStatus != 0);
        }

        //-----------------------------------------------------------------------------
        //
        //
        bool InsertDBAccess::close()
        {
	        return 0;
        }


        //-----------------------------------------------------------------------------
        //
        //
        bool InsertDBAccess::EvaluateLastStatus(char * modName, int line)
        {
	        printf("Error occurred in %s, line %d\n", modName, line); 

	        return true;
        }


        //-----------------------------------------------------------------------------
        //
        //
        bool InsertDBAccess::TableName(const char *name)
        {
	        m_TableName = name;
	        return true;
        }


        //-----------------------------------------------------------------------------
        //
        //
        std::string InsertDBAccess::TableName()
        {
	        return m_TableName;
        }


        //-----------------------------------------------------------------------------
        //
        //
        long InsertDBAccess::LastStatus()
        {
	        return m_LastStatus;
        }


        //-----------------------------------------------------------------------------
        //
        //
        bool InsertDBAccess::bind(	const char *name, const void *addr, int type, 
							        int fieldLen,BindVars &bindVector)
        {
	        bindVector.add(name, addr, type, fieldLen, NULL, 0);

	        return true;
        }

 
} // TA_App

