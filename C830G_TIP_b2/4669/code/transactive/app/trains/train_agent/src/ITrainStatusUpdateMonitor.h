/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/ITrainStatusUpdateMonitor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

///////////////////////////////////////////////////////////
//  ITrainStatusUpdateMonitor.h
//  Implementation of the Interface ITrainStatusUpdateMonitor
//  Created on:      11-Dec-2003 10:05:04 AM
///////////////////////////////////////////////////////////

#if !defined(ITrainStatusUpdateMonitor_39975416_C73D_461f_B8B4_C6FF0D0CEEBA__INCLUDED_)
#define ITrainStatusUpdateMonitor_39975416_C73D_461f_B8B4_C6FF0D0CEEBA__INCLUDED_

/**
 * This interface defines the call backs used to notify a client of changes to the
 * train status.  It will be used by the TrainAgent to synchronise the primary and
 * secondary train agents and to notify other location agents.
 */
namespace TA_IRS_App
{
	class ITrainStatusUpdateMonitor
	{

	public:
		/// @todo fix for degraded and monitor mode support - change status's; new method

		enum EStatusField {NEW_TRAIN,TRAIN_ACTIVE_TSI_FIELD,TRAIN_LOCATION_FIELD,OTHER};

		/// For Train Selection Component consumers or TrainDetailsUpdate
		virtual void onTrainStatusUpdate(unsigned int trainID, const TrainStatus::TrainInformation & trainInfo, TA_IRS_App::ITrainStatusUpdateMonitor::EStatusField field = OTHER) =0;
		virtual void onCctvZoneUpdate(ATSTrainMessage & event) = 0;

		/// For state synchronisation between occ <-> stations, and control <-> monitor
// move to tsm...		virtual void onTrainStateSync(const TrainStatus::TrainInformation & trainInfo) =0;

//		virtual void onTrainLocationChange(unsigned int trainID, const TrainStatus::TrainInformation & trainInfo, TA_IRS_App::ITrainStatusUpdateMonitor::EStatusField field = OTHER) =0;

	};
};
#endif // !defined(ITrainStatusUpdateMonitor_39975416_C73D_461f_B8B4_C6FF0D0CEEBA__INCLUDED_)
