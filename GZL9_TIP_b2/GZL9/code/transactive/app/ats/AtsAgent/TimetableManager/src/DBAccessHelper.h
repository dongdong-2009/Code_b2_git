/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: g:\depot\GZL6_TIP\3002\transactive\app\ats\AtsAgent\SignallingProcessors\src\DBAccessHelper.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/17 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  */

#ifndef _DB_ACCESS_HELPER_H_
#define _DB_ACCESS_HELPER_H_
#include "core/data_access_interface/ats/src/AtsPlannedDataFactory.h"
#include "core/data_access_interface/ats/src/AtsPracticalDataFactory.h"

using namespace TA_IRS_Core;

namespace TA_IRS_App
{
	class DBAccessHelper
	{
	public:	
		/**
		 * Constructor
		 **/
		DBAccessHelper();

		/**
		 * Destructor
		 **/
		~DBAccessHelper();

		void createPraticalTrainData( PracticalTrainDataInfo* practicalData );
		void createPlannedTrainData( PlannedTrainDataInfo* plannedData );

	private:
		/**
		 * Disable copy and assignment operator
		 **/
		DBAccessHelper( const DBAccessHelper& );
		DBAccessHelper& operator= ( const DBAccessHelper& );

	};
}
#endif