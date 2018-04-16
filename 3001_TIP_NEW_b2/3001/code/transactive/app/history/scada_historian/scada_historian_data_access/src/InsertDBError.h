/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/scada_historian/scada_historian_data_access/src/InsertDBError.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// InsertDBError.h: interface for the InsertDBError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBERROR_H__3BCAA0C7_B786_42F4_910F_EEF6A049B2D5__INCLUDED_)
#define AFX_DBERROR_H__3BCAA0C7_B786_42F4_910F_EEF6A049B2D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>
#include "app/history/scada_historian/scada_historian_data_access/src/InsertSavePtr.h"


namespace TA_App
{
    
        class InsertDBError : public RefCount  
        {
        public:
	        void reason(const char *n);
	        void module(const char *name);
	        void line(int n);
	        void status(long n);

	        const char *reason();
	        const char *module();
	        int			line();
	        long		status();

	        InsertDBError();
	        virtual ~InsertDBError();

        private:
	        std::string	m_module;
	        int		    m_line;
	        long	    m_LastStatus;
	        std::string	m_reason;
        };

        typedef Ptr<InsertDBError>	DBErrorPtr;

    
} // TA_App



#endif // !defined(AFX_DBERROR_H__3BCAA0C7_B786_42F4_910F_EEF6A049B2D5__INCLUDED_)

