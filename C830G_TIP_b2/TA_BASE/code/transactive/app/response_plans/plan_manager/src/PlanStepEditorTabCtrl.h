/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditorTabCtrl.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Manages the General and Parameters tabs in the Step Editor. Allows individual tabs to be flagged
  * as disabled when adding the corresponding property page.
  *
  **/

#if !defined(AFX_PLANSTEPEDITORTABCTRL_H__809DD982_5C6C_4050_8154_BFBF0C127CF0__INCLUDED_)
#define AFX_PLANSTEPEDITORTABCTRL_H__809DD982_5C6C_4050_8154_BFBF0C127CF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "bus/mfc_extensions/src/tab_control_with_disable/TabCtrlWithDisable.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditorTabCtrl window

    class CPlanStepEditorTabCtrl : public CTabCtrlWithDisable
    {
    // Construction/destruction
    public:
        CPlanStepEditorTabCtrl();
        virtual ~CPlanStepEditorTabCtrl();


    // Operations
    public:
        void DisableTab(int tabIdx);
        virtual BOOL IsTabEnabled(int tabIdx);


    // Attributes
    private:
        std::vector<int> m_disabledTabs;


    // Overrides
    public:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditorTabCtrl)
        // }}AFX_VIRTUAL

        // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanStepEditorTabCtrl)
        // }}AFX_MSG

        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANSTEPEDITORTABCTRL_H__809DD982_5C6C_4050_8154_BFBF0C127CF0__INCLUDED_)
