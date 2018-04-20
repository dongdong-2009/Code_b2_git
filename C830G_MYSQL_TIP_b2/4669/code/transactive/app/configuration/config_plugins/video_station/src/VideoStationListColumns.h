/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/video_station/src/VideoStationListColumns.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Encapsulates the column specific information for records, to allow the ListView class
  * to be fully generic (interfacing with an arbitrary number of columns as defined in this class).
  * In this way the customisable elements of the list view are more centralised (separate)
  *
  */

#pragma once

#include "app/configuration/config_plugins/config_plugin_helper/src/ListColumnsBase.h"

class VideoStationListColumns 
:
public ListColumnsBase<TA_IRS_App::VideoStationDatabaseAccessor::VideoStationRecord, VideoStationListColumns>
{
public:

    VideoStationListColumns();

    // getAssociatedEquipmentEntities() isn't covered in the columns
    enum EColumn
    {
        COL_MOD_STATUS,
        COL_KEY,                // IVideoStation::getKey()
        COL_LOCATION,           // IVideoStation::getLocationKey()
        COL_MAX        
    };    

    static DWORD getDefaultSortColumn() { return COL_KEY; }

    /**
     * getFieldFromRecord
     *
     * @return a string representing the specific field of this record corresponding to the input row
     * @param column the column defining which field we want to extract from the record
     * @param details the record for which the reference to a member is returned
     */
    static const CString getFieldFromRecord(DWORD column, const TA_IRS_App::VideoStationDatabaseAccessor::VideoStationRecord& details);
};
