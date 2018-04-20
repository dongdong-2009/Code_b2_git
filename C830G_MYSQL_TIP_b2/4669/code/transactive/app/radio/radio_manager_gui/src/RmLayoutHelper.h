/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RmLayoutHelper.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Central point for setting up default characteristics of controls
  * 
  */

#pragma once

#include <list>

namespace TA_Base_Bus
{
    class FixedHeaderCtrl;
}

namespace TA_IRS_App
{
    class RmLayoutHelper
    {
    public:
        static RmLayoutHelper& getInstance();
        virtual ~RmLayoutHelper();

        /**
         * Applies the default layout settings to the specified list control
         *
         * @param listControl defines the control to receive the formatting
         * @param lastColumnIndex defines the last column for this list control, 
         *          this isn't necessarily the actual last column (in case
         *          the user wants to have hidden columns at end of control, as
         *          occurs within radio manager).  Set to -1 to change the
         *          width of the actual last column..
         */
        void layoutListControl(CListCtrl& listControl, int lastColumnIndex = -1);

    private:
        
        typedef std::list<TA_Base_Bus::FixedHeaderCtrl*> HeaderCtrlList;
        // Pool of fixed header column size controllers
        HeaderCtrlList    m_headerControls;
    };

}   // end namespace TA_IRS_App
