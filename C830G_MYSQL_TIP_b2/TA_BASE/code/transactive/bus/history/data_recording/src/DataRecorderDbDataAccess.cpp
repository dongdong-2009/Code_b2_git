/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/history/data_recording/src/DataRecorderDbDataAccess.cpp $
 * @author:  Anita Lee
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 *
 */

#ifdef __WIN32__
#pragma warning (disable : 4786)
#endif

#include <sstream>

#include "bus/history/data_recording/src/DataRecorderDbDataAccess.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/data_recording_dai/src/LivePropertyAccessFactory.h"
#include "core/data_access_interface/data_recording_dai/src/RecordableItemAccessFactory.h"
#include "core/data_access_interface/data_recording_dai/src/IRecordableData.h"
#include "core/data_access_interface/data_recording_dai/src/IRecordableItem.h"


namespace TA_Base_Bus
{

    std::vector<TA_Base_Core::IRecordableDataPtr> DataRecorderDbDataAccess::getRecordableDataByType( unsigned long typeKey )
	{
        FUNCTION_ENTRY("getRecordableDataByType");

        std::vector<TA_Base_Core::ILivePropertyDataPtr> liveProperties = TA_Base_Core::LivePropertyAccessFactory::getInstance().getLivePropertiesByType( typeKey );
        std::vector<TA_Base_Core::ILivePropertyDataPtr>::iterator iter = liveProperties.begin();

        // Cast ILivePropertyDataPtr to IRecordableDataPtr
        std::vector<TA_Base_Core::IRecordableDataPtr> data;
        for(; iter != liveProperties.end(); ++iter)
        {
            TA_Base_Core::IRecordableDataPtr recordableData = *iter;
            data.push_back(recordableData);
        }

        FUNCTION_EXIT;
        return data;
	}


    std::vector<TA_Base_Core::IRecordableItemPtr> DataRecorderDbDataAccess::getRecordableItemsByType( unsigned long typeKey )
	{
        FUNCTION_ENTRY("getRecordableItemsByType");
        std::vector<TA_Base_Core::IRecordableItemPtr> items = TA_Base_Core::RecordableItemAccessFactory::getInstance().getRecordableItemsByType( typeKey );;

        FUNCTION_EXIT;
        return items;
	}
}

