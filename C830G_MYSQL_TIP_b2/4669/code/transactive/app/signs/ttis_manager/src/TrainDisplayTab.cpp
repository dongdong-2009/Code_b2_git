/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/TrainDisplayTab.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * The tab that holds the pre-defined and free text message lists
  *
  */

#include "stdafx.h"
#include "TTISManager.h"
#include "TrainDisplayTab.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CTrainDisplayTab

    CTrainDisplayTab::CTrainDisplayTab() : CTabDialogCtrl()
    {
        FUNCTION_ENTRY( "CTrainDisplayTab" );

        AddPage(m_predefinedPage,IDD_TRAIN_SELECT_PREDEFINED,CString("Predefined"));
        AddPage(m_trainSelectFreeTextPage,IDD_TRAIN_SELECT_FREETEXT,CString("Ad Hoc"));

        FUNCTION_EXIT;
    }


    CTrainDisplayTab::~CTrainDisplayTab()
    {
        FUNCTION_ENTRY( "~CTrainDisplayTab" );
        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(CTrainDisplayTab, CTabDialogCtrl)
        // {{AFX_MSG_MAP(CTrainDisplayTab)
        ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelchange)
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CTrainDisplayTab message handlers

    void CTrainDisplayTab::Init()
    {
        FUNCTION_ENTRY( "Init" );

        m_predefinedPage.Init();
        m_trainSelectFreeTextPage.Init();

        FUNCTION_EXIT;
    }


    CTrainSelectPredefinedPage& CTrainDisplayTab::getPredefinedPage()
    {
        FUNCTION_ENTRY( "getPredefinedPage" );
        FUNCTION_EXIT;
        return m_predefinedPage;
    }


    CTrainSelectFreeTextPage& CTrainDisplayTab::getFreeTextPage()
    {
        FUNCTION_ENTRY( "getFreeTextPage" );
        FUNCTION_EXIT;
        return m_trainSelectFreeTextPage;
    }


    BOOL CTrainDisplayTab::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "OnSelchange" );

        BOOL res = CTabDialogCtrl::OnSelchange(pNMHDR, pResult);

        CDialog* newPage = getActiveDialog();

        if (newPage == &m_predefinedPage)
        {
            m_predefinedPage.windowShown();
        }
        else
        {
            // free text page
            m_trainSelectFreeTextPage.windowShown();
        }

        FUNCTION_EXIT;
        return res;
    }


    // TD 15349
    // zhou yuan++
    bool CTrainDisplayTab::findAndSelectStationMessage(const std::string& messageName)
    {
        FUNCTION_ENTRY( "findAndSelectStationMessage" );

        bool ret = false;
        if(m_predefinedPage.findAndSelectStationMessage(messageName))
        {
            SetCurSel(0);
            m_predefinedPage.windowShown();
            ret    = true;
        }
        else if(m_trainSelectFreeTextPage.findAndSelectStationMessage(messageName))
        {
            SetCurSel(1);
            m_trainSelectFreeTextPage.windowShown();
            ret = true;
        }

        FUNCTION_EXIT;
        return ret;
    }


    void CTrainDisplayTab::setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener)
    {
        FUNCTION_ENTRY( "setMessageSelectionListener" );

        m_predefinedPage.setMessageSelectionListener(messageSelectionListener);
        m_trainSelectFreeTextPage.setMessageSelectionListener(messageSelectionListener);

        FUNCTION_EXIT;
    }


    // ++zhou yuan

} // TA_IRS_App
