/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/pa_zone_group/src/ZoneListHelper.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Provides helper functions for formatting and updating of a CListCtrl control
  * that is responsible for displaying a list of zones
  *
  */

#pragma warning (disable : 4284)

#include "StdAfx.h"
#include "ZoneListHelper.h"
#include "core/data_access_interface/src/DatabaseKey.h"

#include <sstream>

using namespace TA_Base_Bus;
using namespace TA_IRS_Bus;

namespace ZoneListHelper
{
    void formatListControl(CListCtrl& control)
    {
        std::vector<TA_Base_App::MultipleChangeDialog::HeaderProperties> hps = getZoneListHeaderProps();

        for (UINT i = 0; i < hps.size(); i ++)
        {
            int nCol = control.InsertColumn(i, 
                                hps[i].label.c_str(), 
                                hps[i].format, 
                                hps[i].width);

            // Must re-apply the width parameter in certain cases
            if (LVSCW_AUTOSIZE_USEHEADER == hps[i].width)
            {
                control.SetColumnWidth(nCol, hps[i].width);
                // Must account for the scroll bar manually
                int width = control.GetColumnWidth(nCol) - GetSystemMetrics(SM_CXVSCROLL);
                control.SetColumnWidth(nCol, width);
            }
        }    
    }


    std::vector<TA_Base_App::MultipleChangeDialog::HeaderProperties> getZoneListHeaderProps()
    {
        // *NOTE* Update the function getStringElements if you change this
        static const std::string STN_HDR = "STN";
        static const std::string ID_HDR = "ID";
        static const std::string LABEL_HDR = "LABEL";

        static const int STN_WIDTH = 40;
        static const int ID_WIDTH = 35;
        // Expand final column to take up full width
        static const int LABEL_WIDTH = LVSCW_AUTOSIZE_USEHEADER;

        std::vector<TA_Base_App::MultipleChangeDialog::HeaderProperties> result;
        
        TA_Base_App::MultipleChangeDialog::HeaderProperties hp;
        hp.label = STN_HDR;
        hp.format = LVCFMT_LEFT;
        hp.width = STN_WIDTH;
        result.push_back(hp);

        hp.label = ID_HDR;
        hp.format = LVCFMT_LEFT;
        hp.width = ID_WIDTH;
        result.push_back(hp);

        hp.label = LABEL_HDR;
        hp.format = LVCFMT_LEFT;
        hp.width = LABEL_WIDTH;
        result.push_back(hp);

        return result;
    }
    

    std::vector<std::string> getStringElements(const PaZoneRecord& recordData)
    {   
        // *NOTE* Update the function getZoneListHeaderProps if you change this
        std::vector<std::string> result;

        // As defined in getZoneListHeaderProps, station is first
        result.push_back(recordData.m_locationName);
        
        // ID is next
        std::ostringstream ostr;
        if (TA_Base_Core::DatabaseKey::isValidKey(recordData.m_id))
        {
            ostr << recordData.m_id;
        }
        result.push_back(ostr.str());

        // The label is last
        result.push_back(recordData.m_label);
        
        return result;
    }


    int insertNullRow(CListCtrl& control, unsigned long key)
    {
        PaZoneRecord nullRecord;

        nullRecord.m_locationName = "???";
        nullRecord.m_id = TA_Base_Core::DatabaseKey::getInvalidKey();

        std::ostringstream ostr;
        ostr << "NULL (pkey=" << key << ")";
        nullRecord.m_label = ostr.str();

        return insertRow(control, nullRecord);
    }


    int insertRow(CListCtrl& control, const PaZoneRecord& recordData)
    {
        std::vector<std::string> entries = getStringElements(recordData);
        
        int nItem = -1;

        if (entries.size() > 0)
        {
            nItem = control.InsertItem(0, entries[0].c_str());
        }
        else
        {
            nItem = control.InsertItem(0, "");
        }

        // Fill in the rest of the row data
        for (UINT i = 1; i < entries.size(); i ++)
        {
            control.SetItemText(nItem, i, entries[i].c_str());
        }

        // Set associated key data
        control.SetItemData(nItem, recordData.m_pKey);

        return nItem;
    }
}

