#ifndef SAMPLECONFIG_H
#define SAMPLECONFIG_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/history/scada_historian/src/SampleConfig.h $
  * @author:   Anita Lee
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class is a History Config that retrieves the samples and 
  * inserts them into the base table.
  *
  */

#ifdef _MSC_VER
	#pragma warning(disable:4290) // C++ Exception Specification ignored
#endif

#include "app/history/scada_historian/src/HistoryConfig.h"
#include "app/history/scada_historian/scada_historian_data_access/src/ArrayInserter.h"
#include "core/threads/src/Thread.h"

using namespace TA_Core;

class SimpleDb;

namespace TA_Core
{
    class IEntityData;
}


namespace TA_App
{
    class SampleConfig : public HistoryConfig
    {

    public:

	    /**
          * SampleConfig
          *
          * Constructor.
		  *
		  * @param entityData Entity data passed from Generic Agent
          *
          */
	    SampleConfig(IEntityData& entityData)
		    throw(HistorianCfgErrorException);

	    /**
          * ~SampleConfig
          *
          * Destructor.
		  *
		  */
	    virtual ~SampleConfig();

        /**
          * process
          *
          * Calls a database procedure to retrieve the samples and insert
          * them into the appropriate table.
		  *
		  * @param timestamp The most recent timestamp
          *
          */
	    virtual void process(time_t timestamp);
		    
    private:

	    ArrayInserter* m_dataInserter;
        SimpleDb* m_database;
	    std::string m_procedureName;    // Procedure to archive the base table
    };

} // TA_App

#endif // SAMPLECONFIG_H
