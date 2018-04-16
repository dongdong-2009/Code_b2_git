
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_manager/src/CTelephoneDirectLinePage.cpp $
  * @author:  Catherine Loh
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * TelephoneDirectLinePage.cpp 
  *
  * Class implements the search functionality for the telephone manager
  * It handles the "Direct Lines" Property Page.
  */

#include "app/telephone/telephone_manager/src/stdafx.h"
#include "app/telephone/telephone_manager/src/TelephoneApp.h"
#include "app/telephone/telephone_manager/src/CTelephoneDirectLinePage.h"
#include "app/telephone/telephone_manager/src/CTelephoneManagerDlg.h"
#include "app/telephone/telephone_manager/src/TelephoneManagerUserEvents.h"
#include "app/telephone/telephone_manager/src/ItaTelephoneManagerSummary.h"

#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibrary.h"
#include "bus/telephone/telephone_direct_lines_library/src/TelephoneDirectLinesLibrary.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibraryException.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 

#include "core/utilities/src/TAASSERT.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;


class CTelephoneDirectLinePageInitialThread : public TA_Base_Core::Thread
{
public:
    CTelephoneDirectLinePageInitialThread( CTelephoneDirectLinePage& page, CTelephoneManagerDlg* pMainWnd )
        : m_page( page ),
          m_pMainWnd( pMainWnd) {};
    ~CTelephoneDirectLinePageInitialThread() { terminateAndWait(); };
    virtual void run()
    {
        m_page.SetupTabControl();
        m_page.SetupHotButtons();
        m_page.UpdateRights( m_pMainWnd );
        m_page.DisplayButtons();
    }
    virtual void terminate() {};
private:
    CTelephoneDirectLinePage& m_page;
    CTelephoneManagerDlg* m_pMainWnd;
};


IMPLEMENT_DYNCREATE(CTelephoneDirectLinePage, CPropertyPage)

CTelephoneDirectLinePage::CTelephoneDirectLinePage()
    : CPropertyPage(CTelephoneDirectLinePage::IDD),
      m_pTransactiveInterfaces(NULL),
      m_initialThread( NULL )
{
    FUNCTION_ENTRY( "CTelephoneDirectLinePage" );

    // {{AFX_DATA_INIT(CTelephoneDirectLinePage)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CTelephoneDirectLinePage::~CTelephoneDirectLinePage()
{
    FUNCTION_ENTRY( "~CTelephoneDirectLinePage" );

    delete m_initialThread;
    m_initialThread = NULL;

    FUNCTION_EXIT;
}


void CTelephoneDirectLinePage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPropertyPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CTelephoneDirectLinePage)
    DDX_Control(pDX, IDC_DIRECT_LINE_TAB, m_TabControl);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CTelephoneDirectLinePage, CPropertyPage)
    // {{AFX_MSG_MAP(CTelephoneDirectLinePage)
    ON_NOTIFY(TCN_SELCHANGE, IDC_DIRECT_LINE_TAB, OnSelchangeTab)
    // }}AFX_MSG_MAP
    ON_COMMAND_RANGE(IDC_DIRECT_LINE_BUTTON_1, IDC_DIRECT_LINE_BUTTON_36, OnDirectButtonPress)
    ON_COMMAND_RANGE(IDC_HOT_BUTTON1, IDC_HOT_BUTTON8, OnHotButtonPress)
    ON_MESSAGE(WM_DIRECTLINES_MODIFIED, OnSpeedDialButtonModified)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTelephoneDirectLinePage message handlers

BOOL CTelephoneDirectLinePage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPropertyPage::OnInitDialog();

    CTelephoneManagerDlg* pMainWnd = dynamic_cast<CTelephoneManagerDlg*>(AfxGetMainWnd());
    ASSERT_VALID(pMainWnd);

    m_initialThread = new CTelephoneDirectLinePageInitialThread( *this, pMainWnd );
    m_initialThread->start();

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CTelephoneDirectLinePage::UpdateRights( CTelephoneManagerDlg* pMainWnd )
{
    FUNCTION_ENTRY( "UpdateRights" );

    pMainWnd->GetDlgItem(IDC_SELECTED_NUMBER)->EnableWindow(FALSE);
    pMainWnd->GetDlgItem(IDC_SELECTED_LOCATION)->EnableWindow(FALSE);
    pMainWnd->GetDlgItem(IDC_SELECTED_NAME)->EnableWindow(FALSE);
    pMainWnd->GetDlgItem(IDC_SELECTED_DIRECTORY)->EnableWindow(FALSE);

    FUNCTION_EXIT;
}


void CTelephoneDirectLinePage::DisplayButtons()
{
    FUNCTION_ENTRY( "DisplayButtons" );

    TA_ASSERT(m_pTransactiveInterfaces != NULL, "m_pTransactiveInterfaces is NULL; Call setItaSummary first.");

    unsigned int i = 0;

    for(i = IDC_DIRECT_LINE_BUTTON_1; i <= IDC_DIRECT_LINE_BUTTON_36; ++i)
    {
        SetDlgItemText(i, _T(""));
    }

    map<int, TelephoneDirectoryEntryData>* pBtnMap = NULL;
    string setName;

    int nSelect = m_TabControl.GetCurFocus();
    switch(nSelect)
    {
    case 0:
        pBtnMap = &m_userEntries;
        setName = TelephoneDirectLinesLibrary::SETNAME_USER;
        break;

    case 1:
        pBtnMap = &m_stationsEntries;
        setName = TelephoneDirectLinesLibrary::SETNAME_STATIONS;
        break;

    case 2:
        pBtnMap = &m_OCREntries;
        setName = TelephoneDirectLinesLibrary::SETNAME_OCR;
        break;

    case 3:
        pBtnMap = &m_DCREntries;
        setName = TelephoneDirectLinesLibrary::SETNAME_DCR;
        break;

    case 4:
        pBtnMap = &m_otherEntries;
        setName = TelephoneDirectLinesLibrary::SETNAME_OTHERS;
        break;

    case 5:
        pBtnMap = &m_PCPEntries;
        setName = TelephoneDirectLinesLibrary::SETNAME_PCP;
        break;

    default:
        TA_ASSERT(false, "Unknown tab selected!");
    }
	
    vector<TelephoneSpeedDialButtonDetails> btnDetails;
    TelephoneDirectLinesLibrary::getButtonSet(btnDetails, m_pTransactiveInterfaces->getMySessionId(), setName);

	//libo++
	unsigned int nBntNumber =  btnDetails.size();
    TelephoneDirectoryEntryDataMap entryDataMap;
	if(nBntNumber <=0)
	{
		FUNCTION_EXIT;
		return;
	}
	std::string sNumberScope;
	sNumberScope = "  ";
	for(unsigned int n = 0; n < nBntNumber; n++)
    {
		sNumberScope += btnDetails[n].telephoneNumber;
		if(n != nBntNumber-1)
			sNumberScope += " , ";
	}
	sNumberScope += "  ";
	try
	{
		entryDataMap = TelephoneDirectoryLibrary::getEntryByNumberScope(sNumberScope);
	}
	catch(TelephoneDirectoryLibraryException& e)
    {
        std::ostringstream reason;
        reason << "TelephoneDirectoryLibraryException caught [" << e.what() << "]";
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, reason.str().c_str());
        FUNCTION_EXIT;
        return;
    }
	//++libo

    for(i=0; i < btnDetails.size(); ++i)
    {
        SetDlgItemText(btnDetails[i].buttonPosition + IDC_DIRECT_LINE_BUTTON_1 - 1, btnDetails[i].label.c_str());
		//libo++
 		TelephoneDirectoryEntryDataMap::iterator  entryDataIt = entryDataMap.find(btnDetails[i].telephoneNumber);
		if(entryDataIt !=  entryDataMap.end())
		{
			(*pBtnMap)[btnDetails[i].buttonPosition + IDC_DIRECT_LINE_BUTTON_1 - 1] = *(entryDataIt->second);
			delete entryDataIt->second;
		}
		//++libo
    }

    FUNCTION_EXIT;
}


LRESULT CTelephoneDirectLinePage::OnSpeedDialButtonModified(WPARAM pdetails, LPARAM unused)
{
    FUNCTION_ENTRY( "OnSpeedDialButtonModified" );

    TelephoneSpeedDialButtonDetails* pBtnDetails = (TelephoneSpeedDialButtonDetails*) pdetails;

    if (pBtnDetails == NULL)
    {
        FUNCTION_EXIT;
        return 0L;
    }

    map<int, TelephoneDirectoryEntryData>& btnMap = m_Hotlines;
    unsigned int resourceId = 0;

    if(pBtnDetails->SetName == TelephoneDirectLinesLibrary::SETNAME_HOTLINES)
    {
        btnMap = m_Hotlines;
        resourceId = pBtnDetails->buttonPosition + IDC_HOT_BUTTON1 - 1;
    }
    else
    {
        resourceId = pBtnDetails->buttonPosition + IDC_DIRECT_LINE_BUTTON_1 - 1;

        if(pBtnDetails->SetName == TelephoneDirectLinesLibrary::SETNAME_USER)
        {
            btnMap = m_userEntries;
        }
        else if(pBtnDetails->SetName == TelephoneDirectLinesLibrary::SETNAME_STATIONS)
        {
            btnMap = m_stationsEntries;
        }
        else if(pBtnDetails->SetName == TelephoneDirectLinesLibrary::SETNAME_OCR)
        {
            btnMap = m_OCREntries;
        }
        else if(pBtnDetails->SetName == TelephoneDirectLinesLibrary::SETNAME_DCR)
        {
            btnMap = m_DCREntries;
        }
        else if(pBtnDetails->SetName == TelephoneDirectLinesLibrary::SETNAME_OTHERS)
        {
            btnMap = m_otherEntries;
        }
        else
        {
            btnMap = m_PCPEntries;
        }
    }

    if(pBtnDetails->label.empty() && pBtnDetails->telephoneNumber.empty())
    {
        if(btnMap.find(resourceId) != btnMap.end())
        {
            btnMap.erase(resourceId);
        }
    }
    else
    {
        // Retrieve record button record from DB
        TelephoneDirectoryEntryData* entryData;
        try
        {
            entryData = TelephoneDirectoryLibrary::getEntryByNumber(pBtnDetails->telephoneNumber);
            if(entryData != NULL)
            {
                btnMap[resourceId] = *entryData;
                delete entryData;
                entryData = NULL;
            }
        }
        catch(TelephoneDirectoryLibraryException&)
        {
            // TD14164 ++
            /*AfxMessageBox("Error downloading information from database. Please try again!");*/
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_620007);
            // ++ TD14164
            if(pBtnDetails)
            {
                delete pBtnDetails;
                pBtnDetails = NULL;
            }

            FUNCTION_EXIT;
            return false;
        }
    }
	SetDlgItemText(resourceId, pBtnDetails->label.c_str());//cl-17635-lkm
    delete pBtnDetails;
    pBtnDetails = NULL;

    FUNCTION_EXIT;
    return 0L;
}


void CTelephoneDirectLinePage::OnHotButtonPress(UINT ctrlID)
{
    FUNCTION_ENTRY( "OnHotButtonPress" );

    TelephoneSpeedDialButtonDetails btndetails =
            TelephoneDirectLinesLibrary::getButtonDetails(m_pTransactiveInterfaces->getMySessionId(), TelephoneDirectLinesLibrary::SETNAME_HOTLINES, ctrlID - IDC_HOT_BUTTON1 + 1);

    CWnd *pMainWnd = AfxGetMainWnd();
    ASSERT_VALID(pMainWnd);
    if(m_Hotlines.empty() == false && m_Hotlines.find(ctrlID) != m_Hotlines.end())
    {
        pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, m_Hotlines[ctrlID].fullName.c_str());
        pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER, m_Hotlines[ctrlID].number.c_str());
        pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, m_Hotlines[ctrlID].location.c_str());
        pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, m_Hotlines[ctrlID].directoryDataName.c_str());
    }
    else
    {
        if(btndetails.buttonPosition != 0) // There is some data
        {
            pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, btndetails.label.c_str());
            pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER,btndetails.telephoneNumber.c_str());
        }
        else
        {
            pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, "");
            pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER,"");
        }
        pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, "");
        pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, "");
    }

    FUNCTION_EXIT;
}


void CTelephoneDirectLinePage::SetupTabControl()
{
    FUNCTION_ENTRY( "SetupTabControl" );

    m_TabControl.InsertItem(0, TelephoneDirectLinesLibrary::SETNAME_USER.c_str());
    m_TabControl.InsertItem(1, TelephoneDirectLinesLibrary::SETNAME_STATIONS.c_str());
    m_TabControl.InsertItem(2, TelephoneDirectLinesLibrary::SETNAME_OCR.c_str());
    m_TabControl.InsertItem(3, TelephoneDirectLinesLibrary::SETNAME_DCR.c_str());
    m_TabControl.InsertItem(4, TelephoneDirectLinesLibrary::SETNAME_OTHERS.c_str());
    m_TabControl.InsertItem(5, TelephoneDirectLinesLibrary::SETNAME_PCP.c_str());

    FUNCTION_EXIT;
}


void CTelephoneDirectLinePage::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnSelchangeTab" );

    DisplayButtons();
    CTelephoneManagerDlg* pMainWnd = dynamic_cast<CTelephoneManagerDlg*>(AfxGetMainWnd());
    ASSERT_VALID(pMainWnd);

    pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, _T(""));
    pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER, _T(""));
    pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, _T(""));
    pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, _T(""));

    FUNCTION_EXIT;
}


void CTelephoneDirectLinePage::SetupHotButtons()
{
    FUNCTION_ENTRY( "SetupHotButtons" );

    TA_ASSERT(m_pTransactiveInterfaces != NULL, "m_pTransactiveInterfaces is NULL");

    unsigned int i = 0;

    for(i=0; i <= (IDC_HOT_BUTTON8 - IDC_HOT_BUTTON1); ++i)
    {
        SetDlgItemText(IDC_HOT_BUTTON1 + i, _T(""));
    }

    vector<TelephoneSpeedDialButtonDetails> btnSet;
    TelephoneDirectLinesLibrary::getButtonSet(btnSet, m_pTransactiveInterfaces->getMySessionId(), TelephoneDirectLinesLibrary::SETNAME_HOTLINES);
    for(i = 0; i < btnSet.size(); i++)
    {
        SetDlgItemText(btnSet[i].buttonPosition + IDC_HOT_BUTTON1-1, btnSet[i].label.c_str());

        TelephoneDirectoryEntryData* entryData = NULL;
        try
        {
            // entryData = TelephoneDirectoryLibrary::getEntryByNumber(btnSet[i].telephoneNumber);
            // if(entryData)
            {
                // m_Hotlines[btnSet[i].buttonPosition + IDC_HOT_BUTTON1 - 1] = *entryData;
                // delete entryData;
            }
        }
        catch(TelephoneDirectoryLibraryException& e)
        {
            std::ostringstream reason;
            reason << "TelephoneDirectoryLibraryException caught [" << e.what() << "]";
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, reason.str().c_str());

            if(entryData)
            {
                delete entryData;
                entryData = NULL;
            }

            FUNCTION_EXIT;
            return;
        }
    }

    FUNCTION_EXIT;
}


void CTelephoneDirectLinePage::OnDirectButtonPress(UINT ctrlID)
{
    FUNCTION_ENTRY( "OnDirectButtonPress" );

    string selSetName ;

    int selected = m_TabControl.GetCurSel();

    std::map<int, TelephoneDirectoryEntryData> &btnMap = m_userEntries;
    switch(selected)
    {
        case 0:
            btnMap = m_userEntries;
            selSetName = TelephoneDirectLinesLibrary::SETNAME_USER;
            break;

        case 1:
            btnMap = m_stationsEntries;
            selSetName = TelephoneDirectLinesLibrary::SETNAME_STATIONS;
            break;

        case 2:
            btnMap = m_OCREntries;
            selSetName = TelephoneDirectLinesLibrary::SETNAME_OCR;
            break;

        case 3:
            btnMap = m_DCREntries;
            selSetName = TelephoneDirectLinesLibrary::SETNAME_DCR;
            break;

        case 4:
            btnMap = m_otherEntries;
            selSetName = TelephoneDirectLinesLibrary::SETNAME_OTHERS;
            break;

        case 5:
            btnMap = m_PCPEntries;
            selSetName = TelephoneDirectLinesLibrary::SETNAME_PCP;
            break;

        default:
            TA_ASSERT(false, "Unknown tab selected!");
            break;
    };

    TelephoneSpeedDialButtonDetails btndetails =
            TelephoneDirectLinesLibrary::getButtonDetails(m_pTransactiveInterfaces->getMySessionId(), selSetName, ctrlID - IDC_DIRECT_LINE_BUTTON_1 + 1);

    CWnd *pMainWnd = AfxGetMainWnd();
    ASSERT_VALID(pMainWnd);

    if(btnMap.empty() == false && btnMap.find(ctrlID) != btnMap.end())
    {
        pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, btnMap[ctrlID].fullName.c_str());
        pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER, btnMap[ctrlID].number.c_str());
        pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, btnMap[ctrlID].location.c_str());
        pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, btnMap[ctrlID].directoryDataName.c_str());
    }
    else
    {
        if(btndetails.buttonPosition != 0) // There is some data
        {
            pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, btndetails.label.c_str());
            pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER, btndetails.telephoneNumber.c_str());
        }
        else
        {
            pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, "");
            pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER, "");
        }
        pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, "");
        pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, "");
    }

    FUNCTION_EXIT;
}


void CTelephoneDirectLinePage::setItaSummary(ItaTelephoneManagerSummary *ptransactiveInterfaces)
{
    FUNCTION_ENTRY( "setItaSummary" );

    m_pTransactiveInterfaces = ptransactiveInterfaces;

    FUNCTION_EXIT;
}


BOOL CTelephoneDirectLinePage::OnSetActive()
{
    FUNCTION_ENTRY( "OnSetActive" );

    // LIMIN TODO
    static unsigned long ignoreInitialize = 0;
    if ( ignoreInitialize < 2 )
    {
        ignoreInitialize++;

        FUNCTION_EXIT;
        return TRUE;
    }

    CTelephoneManagerDlg* pMainWnd = dynamic_cast<CTelephoneManagerDlg*>(AfxGetMainWnd());
    ASSERT_VALID(pMainWnd);

    UpdateRights( pMainWnd  );

    if ( false == m_Hotlines.empty() )
    {
        m_Hotlines.clear();
    }

    if ( false == m_userEntries.empty() )
    {
        m_userEntries.clear();
    }

    if ( false == m_stationsEntries.empty() )
    {
        m_stationsEntries.clear();
    }

    if ( false == m_OCREntries.empty() )
    {
        m_OCREntries.clear();
    }

    if(false == m_DCREntries.empty())
    {
        m_DCREntries.clear();
    }

    if(false == m_otherEntries.empty())
    {
        m_otherEntries.clear();
    }

    if(false == m_PCPEntries.empty())
    {
        m_PCPEntries.clear();
    }

    SetupHotButtons();
    DisplayButtons();

    pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, _T(""));
    pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER, _T(""));
    pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, _T(""));
    pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, _T(""));

    FUNCTION_EXIT;
    return CPropertyPage::OnSetActive();
}


