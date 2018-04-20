/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/bus/trains/TrainSelectionComponent/src/TrainList.cpp $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This is used by MFCTrainSelectorGUI to maintain a list of trains.
  * it is used so that when new windows register they get the current list given
  * to them.
  *
  * This can be used as an in or outbound list.
  *
  */

#ifdef _MSC_VER
	#pragma warning(disable: 4786)	// identifier was truncated to '255' characters in the debug information
#endif

#include "bus/trains/TrainSelectionComponent/src/TrainList.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{

    using TA_Base_Core::DebugUtil;

	TrainList::TrainList()
		: m_trains()
	{
		FUNCTION_ENTRY( "TrainList" );
	}


	void TrainList::updateList( ITrainSelectorGUI::EAction action,
								const TrainInformationTypes::TrainDetails& trainDetails )
	{
		FUNCTION_ENTRY( "updateList" );

		TA_Base_Core::ThreadGuard guard( m_listLock );

		switch (action)
		{
		case ITrainSelectorGUI::Add:      // Add a new train to the list
		case ITrainSelectorGUI::Update:   // Update a train that exists in the list
			m_trains[trainDetails.trainId] = trainDetails;
			break;
		case ITrainSelectorGUI::Delete:   // Delete a train from the list
			m_trains.erase(trainDetails.trainId);
			break;
		case ITrainSelectorGUI::ClearAll: // clear the list
			m_trains.clear();
			break;
		default:
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
						"Unknown action received %d", action);
			break;
		}

		FUNCTION_EXIT;
	}


} // TA_IRS_Bus


