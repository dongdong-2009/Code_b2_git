/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_monitor_gui/src/SelectedListCtrl.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * List control for Radio Manager, for the selected list 
  *
  */

#pragma once

#include "app/radio/radio_monitor_gui/src/ManagedListCtrl.h"
#include "bus/mfc_extensions/src/mfc_extensions.h"


namespace TA_IRS_App
{

    class SelectedListCtrl
    : 
    public ManagedListCtrl<RadioMonitor::SelectedEntity, TA_Base_Bus::ListCtrlSelNoFocus>
    {
    public:

    protected:
        DECLARE_MESSAGE_MAP()
    };
       
}   // end namespace TA_IRS_App
