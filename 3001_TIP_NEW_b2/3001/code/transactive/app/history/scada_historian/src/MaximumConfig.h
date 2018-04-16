#ifndef MAXIMUMCONFIG_H
#define MAXIMUMCONFIG_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/history/scada_historian/src/MaximumConfig.h $
  * @author:   Anita Lee
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
