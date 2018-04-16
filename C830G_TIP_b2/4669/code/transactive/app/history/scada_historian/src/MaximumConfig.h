#ifndef MAXIMUMCONFIG_H
#define MAXIMUMCONFIG_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/history/scada_historian/src/MaximumConfig.h $
  * @author:   Anita Lee
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class is a History Config that calcuates samples that
  * are maximums of the base table. The maximums are inserted into
  * the database.
  *
  */

#ifdef _MSC_VER
	#pragma warning(disable:4290) // C++ Exception Specification ignored
#endif

#include "app/history/scada_historian/src/HistoryConfig.h"
#include "core/database/src/SimpleDb.h"

using namespace TA_Core;


namespace TA_App
{
    class MaximumConfig : public HistoryConfig
    {

    public:

	    /**
          * MaximumConfig
          *
          * Constructor.
		  *
		  * @param entityData Entity data passed from Generic Agent
          *
          */
	    MaximumConfig(IEntityData& entityData)
		    throw(HistorianCfgErrorException);

	    /**
          * ~MaximumConfig
          *
          * Constructor.
		  *
		  */
	    virtual ~MaximumConfig();

        /**
          * process
          *
          * Calls a database procedure to calculate the maximums and insert
          * them into the appropriate table.
		  *
		  * @param timestamp The most recent timestamp
          *
          */
	    virtual void process(time_t timestamp);
		    
    private:

	    std::string m_procedureName; // procedure to find the maximums

	    SimpleDb* m_database;      
    };

} // TA_App

#endif // MAXIMUMCONFIG_H
