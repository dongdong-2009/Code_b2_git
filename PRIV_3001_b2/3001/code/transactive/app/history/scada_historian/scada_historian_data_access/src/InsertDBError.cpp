/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/scada_historian/scada_historian_data_access/src/InsertDBError.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// InsertDBError.cpp: implementation of the InsertDBError class.
//
//////////////////////////////////////////////////////////////////////

#include "app/history/scada_historian/scada_historian_data_access/src/InsertDBError.h"


namespace TA_App
{
    
        //////////////////////////////////////////////////////////////////////
        // Construction/Destruction
        //////////////////////////////////////////////////////////////////////

        InsertDBError::InsertDBError()
        {

        }

        InsertDBError::~InsertDBError()
        {

        }

        void InsertDBError::module(const char *name)
        {
	        m_module = name;
        }

        void InsertDBError::reason(const char *n)
        {
	        m_reason = n;
        }

        void InsertDBError::line(int n)
        {
	        m_line = n;
        }

        void InsertDBError::status(long n)
        {
	        m_LastStatus = n;
        }


        const char *InsertDBError::module()
        {
	        return m_module.c_str();
        }

        const char *InsertDBError::reason()
        {
	        return m_reason.c_str();
        }

        int InsertDBError::line()
        {
	        return m_line;
        }

        long InsertDBError::status()
        {
	        return m_LastStatus;
        }

    
} // TA_App


