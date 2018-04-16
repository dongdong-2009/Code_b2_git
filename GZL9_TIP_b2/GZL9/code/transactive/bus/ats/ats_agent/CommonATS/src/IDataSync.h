/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\IDataSync.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * A interface use to process Data Synchronization
  */

#ifndef _I_DATA_SYNC_H_
#define _I_DATA_SYNC_H_

namespace TA_IRS_Bus
{
	class IDataUpdate;
	class IDataSync
	{
	public:
		/**
		 * processDataSync
		 *
		 * This method will use for Data sync processing.
		 * 
		 */
		virtual void processDataSync() = 0;

		/**
		 * setDataUpdates
		 *
		 * This method sets the data update interface
		 *
		 * @param dataUpdate : Data Update object
		 */
		virtual void setDataUpdates(IDataUpdate* dataUpdate) = 0;
	};
}
#endif