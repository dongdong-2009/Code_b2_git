/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/scada_historian/scada_historian_data_access/src/OCIAccessExtended.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#ifndef OCIACCESS_EXTENDED
#define OCIACCESS_EXTENDED

#include "app/history/scada_historian/scada_historian_data_access/src/OCIInsertAccess.h"
#include "core/database/src/BindVars.h"


namespace TA_App
{
        class OCIAccessExtended : public OCIInsertAccess
        {
        public:

	        OCIAccessExtended(const char* hostName, const char* databaseName,
		        const char* userName, const char* password, int max_iterations);

	        virtual ~OCIAccessExtended() {}

	        bool execArrayInsert(const std::string& command, long &retCode, BindVars *bindVector = 0, 
		        int bindType = InsertDBAccess::BY_NAME, int commit = 1);

	        void setMaxIterations(int iterations) { m_maxIterations = iterations; }

        protected:

	        int m_maxIterations;
        };

    
} // TA_App

#endif // OCIACCESS_EXTENDED
