#ifndef AVERAGECONFIG_H
#define AVERAGECONFIG_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/history/scada_historian/src/AverageConfig.h $
  * @author:   Anita Lee
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This class is a History Config that calcuates samples that
  * are averages of the base table. The averages are inserted into
  * the database.
  *
  */

#include "app/history/scada_historian/src/HistoryConfig.h"
#include "core/database/src/SimpleDb.h"

#ifdef _MSC_VER
	#pragma warning(disable:4290) // C++ Exception Specification ignored
#endif
using namespace TA_Core;


//
// Forward declarations
//
namespace TA_Core
{
    class IEntityData;
}


namespace TA_App
{
    class AverageConfig : public HistoryConfig
    {

    public:

	    /**
          * AverageConfig
          *
          * Constructor.
		  *
		  * @param entityData Entity data passed from Generic Agent
          *
          */
	    AverageConfig(IEntityData& entityData)
		    throw(HistorianCfgErrorException);

	    /**
          * ~AverageConfig
          *
          * Destructor.
		  *
		  */
	    virtual ~AverageConfig();

        /**
          * process
          *
          * Calls a database procedure to calculate the averages and insert
          * them into the appropriate table.
		  *
		  * @param timestamp The most recent timestamp
          *
          */
	    virtual void process(time_t timestamp);
		    
    private:
    
	    std::string m_procedureName;    // procedure that will average the base samples

	    SimpleDb* m_database;
    };

} // TA_App

#endif // AVERAGECONFIG_H
