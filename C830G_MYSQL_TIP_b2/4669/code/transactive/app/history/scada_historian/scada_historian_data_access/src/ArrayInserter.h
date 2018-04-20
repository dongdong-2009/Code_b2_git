/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/scada_historian/scada_historian_data_access/src/ArrayInserter.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// ArrayInserter

#ifndef ARRAY_INSERTER_H
#define ARRAY_INSERTER_H

#include <string>
#include <vector>
#include "core/database/src/BindVars.h"
#include "app/history/scada_historian/scada_historian_data_access/src/OCIAccessExtended.h"
#include "app/history/scada_historian/src/DataPointData.h"

#define MAX_ITERATIONS 2000

namespace TA_App
{
    
    const std::string TIMESTAMP_FNAME   = ":TIMESTAMP";
    const std::string ENTITYKEY_FNAME   = ":ENTITYKEY";
    const std::string STATUS_FNAME      = ":STATUS";
    const std::string VALUE_FNAME       = ":VALUE";
    const std::string COMMS_ERROR_FNAME = ":COMMS_ERROR";

    class ArrayInserter : private OCIAccessExtended
    {
    public:

	    // CTOR
	    ArrayInserter(const char* tablename, const char* connection, const char* user, const char* passwd, const char* hostname = "");

	    // DTOR
	    virtual ~ArrayInserter();

	    typedef std::vector<DataPointData> DataPointDataList;
	    // Need a new exec method to do array insertion
	    bool insert(DataPointDataList& DpDataList, time_t timestamp);

	    // You MUST open the connection before calling Exec(), otherwise you
        // will get an assertion. You can close the connnection yourself or
        // let the destructor do this
	    bool Open();
	    void Close();

	    // Do we have an open connection?
	    bool IsOpen();
	    
    protected:

	    BindVars* m_bindVars;

    private:

	    // Do we have an open connection?
        bool m_IsOpen;

        std::string m_tableName;
    };
    
} // TA_App

#endif // ARRAY_INSERTER_H

