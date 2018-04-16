/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/radio/radio_monitor_gui/src/SelectedListCtrl.h $
  * @author Jade Lee
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
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
