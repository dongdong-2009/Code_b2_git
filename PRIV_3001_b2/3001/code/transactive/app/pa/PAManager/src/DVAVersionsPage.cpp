/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/DVAVersionsPage.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Controls the DVA version display page
  *
  */

#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/pamanager.h"
#include "app/pa/PAManager/src/DvaVersionsPage.h"
#include "app/pa/PAManager/src/PaErrorHandler.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "core/exceptions/src/PasExceptions.h"
#include "app/pa/PAManager/src/PaLayoutHelper.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa/cachedmaps/src/PaDvaVersionCachedMap.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_IRS_Bus;

const DWORD HEADER_TEXT_COLOUR = RGB(255, 255, 255);
const DWORD HEADER_BACKGROUND_COLOUR = RGB(0, 0, 0);
// This is the amount the list control is extended (width) in order to view full version info
// - not required any more, after change made to only view one DVA set (not both private and public)
// - this means none of the scrolling code is used (required) anymore, but left in nonetheless
const DWORD LIST_WIDTH_EXCESS = 0;

IMPLEMENT_DYNCREATE(DvaVersionsPage, CPropertyPage)

DvaVersionsPage::DvaVersionsPage(PAManagerDlg* paManagerDlg)
: 
CPropertyPage(DvaVersionsPage::IDD)
{
    //{{AFX_DATA_INIT(DvaVersionsPage)
	//}}AFX_DATA_INIT
}

DvaVersionsPage::~DvaVersionsPage()
{
}

void DvaVersionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DvaVersionsPage)
	DDX_Control(pDX, IDC_DVA_HEADER_LST_2, m_secondVersionHeaderList);
	DDX_Control(pDX, IDC_DVA_VERSION_MAIN, m_versionList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DvaVersionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(DvaVersionsPage)
    ON_WM_DESTROY()
	ON_MESSAGE(WM_PA_DVA_VERSION_UPDATE, OnDvaVersionUpdate)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_DVA_HEADER_LST_2, OnItemchangingDvaHeaderLst2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL DvaVersionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();	

    ///////////      
    // Listen out for updates to any ad-hoc messages
    try
    {
        m_dvaVersionChangeSubscriber.addListenerWindow(m_hWnd, true, (CachedConfig::getInstance()->getSchematicLocType() != CachedConfig::LOCATION_TYPE_OCC));
    }
    catch (const std::exception& expWhat)
    {
        // The ad-hoc list items will not be updated (nothing major, just can't view up to date info)
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    
    m_versionList.addHorizontalScrollingChild(m_secondVersionHeaderList);

    m_secondVersionHeaderList.HideScrollBars(LCSB_NCOVERRIDE);

	CachedConfig::ELocationType locationType = CachedConfig::getInstance()->getAgentLocType();
    std::string locationTypeString = "";

    switch(locationType)
    {
    case CachedConfig::LOCATION_TYPE_OCC:
        locationTypeString = "Station DVA Set Versions";
        break;
    case CachedConfig::LOCATION_TYPE_STATION:
        locationTypeString = "Station DVA Set Versions";
        break;
    case CachedConfig::LOCATION_TYPE_DEPOT:
        locationTypeString = "Depot DVA Set Versions";
        break;
    default:
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invaild location");
    }

    SetDlgItemText( IDC_STATIC_DVA_LABEL, locationTypeString.c_str() );
    
    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void DvaVersionsPage::postDialogCreate()
{
    // set up the primary list of versions
    setupMainVersionList();

    setupSecondHeaderList();
}


void DvaVersionsPage::setupSecondHeaderList()
{
    // These show headings for main list - needs to match precisely
    setupListView(m_secondVersionHeaderList, 2, true, LVCFMT_LEFT);

    m_secondVersionHeaderList.SetTextColor(HEADER_TEXT_COLOUR);
    m_secondVersionHeaderList.SetTextBkColor(HEADER_BACKGROUND_COLOUR);
    m_secondVersionHeaderList.SetBkColor(HEADER_BACKGROUND_COLOUR);

    // Add in the list headings before adding in elements
    int nItem = m_secondVersionHeaderList.InsertItem(0, "LOC");
    m_secondVersionHeaderList.SetItemText(nItem, 1, "PA Version Number");
}


void DvaVersionsPage::setupMainVersionList()
{      
    setupListView(m_versionList, 2, true, LVCFMT_LEFT);

    // Now add in the actual version data
    try
    {
        TA_IRS_Bus::DvaVersionRecords allVersionData = TA_IRS_Bus::CachedMaps::getInstance()->getDvaVersionCachedMap().getAllRecords();
        // Add in all the row data
        for (DvaVersionRecordsIt it = allVersionData.begin(); it != allVersionData.end(); it ++)
        {
            m_versionList.addRowData(*it);
        }
    }
    catch (const CORBA::Exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", expWhat._name() );
    }
    catch (const std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception" );
    }
}


void DvaVersionsPage::setupListView(CListCtrl& list, 
                                        DWORD columnCount, 
                                        bool accountForScroll,
                                        DWORD format)
{
    list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    
    // Width is default width as seen in dialog editor
    int width = PaLayoutHelper::getListHeaderWidth(list, false);
    
    // add in excess amount
    width += LIST_WIDTH_EXCESS;    

    const int nLocColWidth = 150;

    for (DWORD nCol = 0; nCol < columnCount; nCol ++)  
    {
        // Keep inserting at the back (so the formatting works
        // - formatting is always left justified on first column no
        // matter what you specify)
        int nItem = list.InsertColumn(nCol, "", format);

        if (0 == nCol)
        {
            // Make the first item expand to fill out any remainder
            list.SetColumnWidth(nItem, nLocColWidth);
        }
        else
        {
            list.SetColumnWidth(nItem, width - nLocColWidth);
        }
    }

    list.SetScrollPos(SB_HORZ, 0);	
}


void DvaVersionsPage::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	    
    try
    {
         m_dvaVersionChangeSubscriber.removeListenerWindow(m_hWnd);
    }
    catch (const CORBA::Exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", expWhat._name() );
    }
    catch (const std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception" );
    }
}


afx_msg LRESULT DvaVersionsPage::OnDvaVersionUpdate(UINT wParam, LONG lParam)
{
    if (!m_dvaVersionChangeSubscriber.isConnectedListenerWindow(m_hWnd))
    {
        // Probably in the closing down stage, receiving last few messages
        // while unsubscribed - do NOT respond to this notification in this case
        return 0;
    }

    try
    {
        const TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords allVersionData 
            = m_dvaVersionChangeSubscriber.getMessageData(wParam, m_hWnd);
        for (int i = 0; i < allVersionData.length(); i ++)
        {
            m_versionList.updateRowData(allVersionData[i]);
        }
    }
    catch (const CORBA::Exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", expWhat._name() );
    }
    catch (const std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception" );
    }
    
    return 0;
}


void DvaVersionsPage::OnItemchangingDvaHeaderLst2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

  	// Only interested in state changes
	if (pNMListView->uChanged != LVIF_STATE) 
	{
		return;
	}

    // Reject the change in state (no selections allowed on header controls)
    *pResult = TRUE;   
}

UINT DvaVersionsPage::getTemplateID()
{
    return IDD;
}
