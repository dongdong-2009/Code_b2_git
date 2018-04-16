//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanEditorToolBarManager.h $
// @author:  Bart Golab
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2012/02/06 16:15:14 $
// Last modified by:  $Author: haijun.li $
//
// <description>

#if !defined(AFX_PLANEDITORTOOLBARMANAGER_H__6554658E_AB51_4BDE_A40B_4AC7B7EB93D2__INCLUDED_)
#define AFX_PLANEDITORTOOLBARMANAGER_H__6554658E_AB51_4BDE_A40B_4AC7B7EB93D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AbstractToolBarManager.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanEditorToolBarManager

    class PlanEditorToolBarManager : public AbstractToolBarManager
    {
    // Types
    public:
        typedef enum
                {
                    TBT_GENERAL = 0,
                    TBT_STEPS,
                    TBT_FLOWCHART,
                    TBT_NONE
                }
                EToolBarType;

    // Operations
    public:
        PlanEditorToolBarManager();
        virtual ~PlanEditorToolBarManager();

        void selectToolBar(EToolBarType toolBarType);

    protected:
        virtual void initialiseToolBarList();
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANEDITORTOOLBARMANAGER_H__6554658E_AB51_4BDE_A40B_4AC7B7EB93D2__INCLUDED_)
