#ifndef MINIMUMCONFIG_H
#define MINIMUMCONFIG_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/history/scada_historian/src/MinimumConfig.h $
  * @author:   Anita Lee
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class is a History Config that calcuates samples that
  * are minimums of the base table. The minimums are inserted into
  * the database.
  *
  */

#ifdef _MSC_VER
	#pragma warning(disable:4290) // C++ Exception Specification ignored
#endif

#include "app/history/scada_historian/src/HistoryConfig.h"
#include "core/database/src/SimpleDb.h"


using namespace TA_Core;


namespace TA_Core
{
    class IEntityData;
}


namespace TA_App
{
    class MinimumConfig : public HistoryConfig
    {

    public:

	    /**
          * MinimumConfig
          *
          * Constructor.
		  *
		  * @param entityData Entity data passed from Generic Agent
          *
          */
	    MinimumConfig(IEntityData& entityData)
		    throw(HistorianCfgErrorException);

	    /**
          * ~MinimumConfig
          *
          * Constructor.
		  *
          */
	    virtual ~MinimumConfig();

        /**
          * process
          *
          * Calls a database procedure to calculate the minimums and insert
          * them into the appropriate table.
		  *
		  * @param timestamp The most recent timestamp
          *
          */
	    virtual void process(time_t timestamp);
		    
    private:

	    std::string m_procedureName;	// procedure to find the minimums from the base samples
	    
	    SimpleDb* m_database;
    };

} // TA_App

#endif // MINIMUMCONFIG_H
