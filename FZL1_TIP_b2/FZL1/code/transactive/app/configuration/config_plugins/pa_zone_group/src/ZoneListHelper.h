/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/pa_zone_group/src/ZoneListHelper.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Provides helper functions for formatting and updating of a CListCtrl control
  * that is responsible for displaying a list of zones
  *
  */

#pragma once

#include "bus/pa/cachedmaps/src/PaZoneCachedMap.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MultipleChangeDialog.h"

namespace ZoneListHelper
{
    /**
     * formatListControl
     *
     * Formats the input list control using default zone list formatting
     *
     */
    void formatListControl(CListCtrl& control);

    /**
     * getZoneListHeaderProps
     *
     * Partner function to getStringElements
     *
     * @return an array of header properties that should be used for a zone CListCtrl
     */
    std::vector<TA_Base_App::MultipleChangeDialog::HeaderProperties> getZoneListHeaderProps();

    /**
     * getStringElements
     *
     * Partner function to getZoneListHeaderProps
     *
     * @return an array of strings defining the data that should be displayed column
     *          by column for a list control row, using the ordering defined by
     *          getZoneListHeaderProps
     */
    std::vector<std::string> getStringElements(const TA_IRS_Bus::PaZoneRecord& recordData);

    /*
     * insertRow
     *
     * @param control the control to have the row inserted
     * @param recordData the data to insert into list
     */
    int insertRow(CListCtrl& control, const TA_IRS_Bus::PaZoneRecord& recordData);

    /*
     * insertNullRow
     *
     * @param control the control to have the row inserted
     * @param key the key of the record that was unable to be fetched (hence is null)
     */
    int insertNullRow(CListCtrl& control, unsigned long key);
};
