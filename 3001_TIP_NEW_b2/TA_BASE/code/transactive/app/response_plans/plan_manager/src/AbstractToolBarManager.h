//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/AbstractToolBarManager.h $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2012/03/28 22:54:51 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#if !defined(AFX_ABSTRACTTOOLBARMANAGER_H__D5AEFAAB_C501_42C5_8355_A81A5E7BFCD7__INCLUDED_)
#define AFX_ABSTRACTTOOLBARMANAGER_H__D5AEFAAB_C501_42C5_8355_A81A5E7BFCD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // AbstractToolBarManager

    class AbstractToolBarManager
    {
    // Operations
    public:
        AbstractToolBarManager();
        virtual ~AbstractToolBarManager();

        virtual void initialise(CFrameWnd *toolBarParent);
        virtual void toggleToolBarDisplay();
        virtual bool isToolBarVisible() const;
        virtual bool isToolBarEnabled() const;
        virtual CRect getBoundingRect(int commandID);
        virtual void updateCmdUI(CCmdUI &cmdUI);

    protected:
        virtual void initialiseToolBarList() = 0;
        virtual CToolBar *addToolBarToList(UINT toolBarResourceId);
        virtual void selectToolBar(int toolBarIdx);

    private:
        CToolBar *createToolBar(UINT toolBarResourceId);


    // Types:
    private:
        typedef std::vector<CToolBar *> ToolBarList;


    // Attributes
    protected:
        CFrameWnd *m_toolBarParent;

        ToolBarList m_toolBars;
        int m_selectedToolBarIdx;

        bool m_isToolBarVisible;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_ABSTRACTTOOLBARMANAGER_H__D5AEFAAB_C501_42C5_8355_A81A5E7BFCD7__INCLUDED_)
