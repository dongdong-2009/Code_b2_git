/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/CTelephoneConfigureDirectLinesPage.cpp $
  * @author:  Mark See
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implements TelephoneConfigureDirectLinesPage 
  * User will be able configure the button settings on the direct line
  * config page. This class will write any user-set information to the 
  * telephoneDirectLineLibrary, to be subsequently written into the database
  * It handles the "Configure Direct Line" Property Page.
  */

#include "app/telephone/telephone_manager/src/stdafx.h"
#include "app/telephone/telephone_manager/src/TelephoneApp.h"
#include "app/telephone/telephone_manager/src/CTelephoneConfigureDirectLinesPage.h"
#include "app/telephone/telephone_manager/src/TelephoneManagerUserEvents.h"
#include "app/telephone/telephone_manager/src/ItaTelephoneManagerSummary.h"
#include "app/telephone/telephone_manager/src/CTelephoneManagerDlg.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibrary.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibraryException.h"
#include "bus/telephone/telephone_direct_lines_library/src/TelephoneDirectLinesLibrary.h"
#include "bus/telephone/telephone_direct_lines_library/src/TelephoneDirectLinesLibraryException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/message/types/TelephoneManagerAudit_MessageTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

const unsigned short CTelephoneConfigureDirectLinesPage::MAX_HOTLINE_BUTTONS = 8;
const unsigned short CTelephoneConfigureDirectLinesPage::MAX_TAB_BUTTONS = 36;

typedef vector<TelephoneDirectoryEntryData*>    TEL_DIR_ENTRY_LIST;
typedef map<int, TelephoneDirectoryEntryData>   RES_TEL_DIR_ENTRY_MAP;

const unsigned int CTelephoneConfigureDirectLinesPage::TAB_BUTTONS_BASE_RESOURCE_ID = IDC_DIRECT_LINE_BUTTON_1 - 1;
const unsigned int CTelephoneConfigureDirectLinesPage::HOTLINE_BUTTONS_BASE_RESOURCE_ID = IDC_HOT_BUTTON1 - 1;


class CTelephoneConfigureDirectLinesPageInitialThread : public TA_Base_Core::Thread
{
public:
    CTelephoneConfigureDirectLinesPageInitialThread( CTelephoneConfigureDirectLinesPage& page, CTelephoneManagerDlg* pMainWnd ) 
        : m_page( page ),
          m_pMainWnd( pMainWnd ) {};
    ~CTelephoneConfigureDirectLinesPageInitialThread() { terminateAndWait(); };
    virtual void run()
    {
        m_page.m_TabControl.InsertItem(0, TelephoneDirectLinesLibrary::SETNAME_USER.c_str());
        m_page.m_TabControl.InsertItem(1, TelephoneDirectLinesLibrary::SETNAME_STATIONS.c_str());
        m_page.m_TabControl.InsertItem(2, TelephoneDirectLinesLibrary::SETNAME_OCR.c_str());
        m_page.m_TabControl.InsertItem(3, TelephoneDirectLinesLibrary::SETNAME_DCR.c_str());
        m_page.m_TabControl.InsertItem(4, TelephoneDirectLinesLibrary::SETNAME_OTHERS.c_str());
        m_page.m_TabControl.InsertItem(5, TelephoneDirectLinesLibrary::SETNAME_PCP.c_str());

        m_page.UpdateRights( m_pMainWnd );
        m_page.SetupHotButtons();
        m_page.DisplayButtons();
    }
    virtual void terminate() {};
private:
    CTelephoneConfigureDirectLinesPage& m_page;
    CTelephoneManagerDlg* m_pMainWnd;
};


TEL_DIR_ENTRY_LIST::iterator findEntryByNumber(TEL_DIR_ENTRY_LIST& iList, string iNumber)
{
    TEL_DIR_ENTRY_LIST::iterator iter = iList.begin();
    while(iter != iList.end())
    {
        if((*iter)->number.compare(iNumber) == 0)
            return iter;

        iter++;
    }

    return iList.end();
}

int findResourceIdByNumber(RES_TEL_DIR_ENTRY_MAP& iMap, string iNumber)
{
    RES_TEL_DIR_ENTRY_MAP::iterator iter = iMap.begin();
    while(iter != iMap.end())
    {
        if((*iter).second.number.compare(iNumber) == 0)
            return (*iter).first;

        iter++;
    }

    return -1;
}

/////////////////////////////////////////////////////////////////////////////
// CTelephoneConfigureDirectLinesPage property page

IMPLEMENT_DYNCREATE(CTelephoneConfigureDirectLinesPage, CPropertyPage)

CTelephoneConfigureDirectLinesPage::CTelephoneConfigureDirectLinesPage()
    : CPropertyPage(CTelephoneConfigureDirectLinesPage::IDD),
      m_isHotButton(false),
      m_buttonNumber(0),
      m_initialThread( NULL )
{
    FUNCTION_ENTRY( "CTelephoneConfigureDirectLinesPage" );

    // {{AFX_DATA_INIT(CTelephoneConfigureDirectLinesPage)
    m_Label = _T("");
    m_Number = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CTelephoneConfigureDirectLinesPage::~CTelephoneConfigureDirectLinesPage()
{
    FUNCTION_ENTRY( "~CTelephoneConfigureDirectLinesPage" );

    delete m_initialThread;
    m_initialThread = NULL;

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectLinesPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPropertyPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CTelephoneConfigureDirectLinesPage)
    DDX_Control(pDX, IDC_DIRECT_LINE_TAB, m_TabControl);
    DDX_Text(pDX, IDC_LABEL_EDIT, m_Label);
    DDV_MaxChars(pDX, m_Label, 10);
    DDX_Text(pDX, IDC_NUMBER_EDIT, m_Number);
    DDV_MaxChars(pDX, m_Number, 8);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectLinesPage::setItaSummary(ItaTelephoneManagerSummary* iSummary)
{
    FUNCTION_ENTRY( "setItaSummary" );

    m_Summary = iSummary;

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectLinesPage::UpdateRights( CTelephoneManagerDlg* pMainWnd )
{
    FUNCTION_ENTRY( "UpdateRights" );

    CTelephoneManagerDlg* app = dynamic_cast<CTelephoneManagerDlg*>(pMainWnd);

    if ( false == pMainWnd->isFunctionPermitted(TA_Base_Bus::aca_TELEPHONE_CONFIGURE_USER_DIRECT_LINE ) )
    {
        CWnd *pWnd = GetDlgItem(IDC_SET_BTN);
        pWnd->EnableWindow(false);
    }
    else
    {
        CWnd *pWnd = GetDlgItem(IDC_SET_BTN);
        // ASSERT_VALID(pMainWnd);
        pWnd->EnableWindow(true);

        pWnd = GetDlgItem(IDC_LABEL_EDIT);
        // ASSERT_VALID(pMainWnd);
        pWnd->EnableWindow(true);

        pWnd = GetDlgItem(IDC_NUMBER_EDIT);
        // ASSERT_VALID(pMainWnd);
        pWnd->EnableWindow(true);
    }

    pMainWnd->GetDlgItem(IDC_SELECTED_NUMBER)->EnableWindow(FALSE);
    pMainWnd->GetDlgItem(IDC_SELECTED_LOCATION)->EnableWindow(FALSE);
    pMainWnd->GetDlgItem(IDC_SELECTED_NAME)->EnableWindow(FALSE);
    pMainWnd->GetDlgItem(IDC_SELECTED_DIRECTORY)->EnableWindow(FALSE);

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectLinesPage::DisplayButtons()
{
    FUNCTION_ENTRY( "DisplayButtons" );

    TA_ASSERT(m_Summary != NULL, "m_Summary is null; Call setItaSummary first.");

    clearTabBtns();

    int selected = m_TabControl.GetCurSel();

    string setName;
    map<int, TelephoneDirectoryEntryData>* pBtnMap = NULL;

    switch(selected)
    {
        case 0:
            setName = TelephoneDirectLinesLibrary::SETNAME_USER;
            pBtnMap = &m_userEntries;
            break;

        case 1:
            setName = TelephoneDirectLinesLibrary::SETNAME_STATIONS;
            pBtnMap = &m_stationsEntries;
            break;

        case 2:
            setName = TelephoneDirectLinesLibrary::SETNAME_OCR;
            pBtnMap = &m_OCREntries;
            break;

        case 3:
            setName = TelephoneDirectLinesLibrary::SETNAME_DCR;
            pBtnMap = &m_DCREntries;
            break;

        case 4:
            setName = TelephoneDirectLinesLibrary::SETNAME_OTHERS;
            pBtnMap = &m_otherEntries;
            break;

        case 5:
            setName = TelephoneDirectLinesLibrary::SETNAME_PCP;
            pBtnMap = &m_PCPEntries;
            break;

        default:
            TA_ASSERT(false, "Unknown Tab selected.");
            break;
    };

    // Retrieve button records from DB
    vector<TelephoneSpeedDialButtonDetails> btnSet;

    try
    {
         TelephoneDirectLinesLibrary::getButtonSet( btnSet, m_Summary->getMySessionId(), setName);
    }
    catch(TelephoneDirectLinesLibraryException&)
    {
        // TD14164 ++
        /*AfxMessageBox("Error reading button information from database!! Button Information Lost!") ;*/
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg.showMsgBox(IDS_UE_620005);
        // ++ TD14164
        FUNCTION_EXIT;
        return;
    }

    updateLabelButtons(TAB_BUTTONS_BASE_RESOURCE_ID, *pBtnMap, btnSet);

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectLinesPage::SetupHotButtons()
{
    FUNCTION_ENTRY( "SetupHotButtons" );

    TA_ASSERT(m_Summary != NULL, "m_Summary is null; Call setItaSummary first.");

    // Retrieve button records from DB
    vector<TelephoneSpeedDialButtonDetails> btnSet;
    try
    {
         TelephoneDirectLinesLibrary::getButtonSet( btnSet, m_Summary->getMySessionId(), TelephoneDirectLinesLibrary::SETNAME_HOTLINES);
    }
    catch(TelephoneDirectLinesLibraryException&)
    {
        // TD14164 ++
        /*AfxMessageBox("Error reading button information from database!! Button Information Lost!") ;*/
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg.showMsgBox(IDS_UE_620005);
        // ++ TD14164
        FUNCTION_EXIT;
        return;
    }

    updateLabelButtons(HOTLINE_BUTTONS_BASE_RESOURCE_ID, m_Hotlines, btnSet);

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CTelephoneConfigureDirectLinesPage, CPropertyPage)
    // {{AFX_MSG_MAP(CTelephoneConfigureDirectLinesPage)
    ON_NOTIFY(TCN_SELCHANGE, IDC_DIRECT_LINE_TAB, OnSelchangeTab)
    ON_BN_CLICKED(IDC_SET_BTN, OnSetButton)
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_DIRECTLINES_MODIFIED, OnSpeedDialButtonModified)
    ON_COMMAND_RANGE(IDC_HOT_BUTTON1, IDC_HOT_BUTTON8, OnHotButtonPress)
    ON_COMMAND_RANGE(IDC_DIRECT_LINE_BUTTON_1, IDC_DIRECT_LINE_BUTTON_36, OnDirectButtonPress)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTelephoneConfigureDirectLinesPage message handlers

BOOL CTelephoneConfigureDirectLinesPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPropertyPage::OnInitDialog();

    CTelephoneManagerDlg* pMainWnd = dynamic_cast<CTelephoneManagerDlg*>(AfxGetMainWnd());
    ASSERT_VALID(pMainWnd);

    m_initialThread = new CTelephoneConfigureDirectLinesPageInitialThread( *this, pMainWnd );
    m_initialThread->start();

    FUNCTION_EXIT;
    return TRUE;
}


void CTelephoneConfigureDirectLinesPage::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnSelchangeTab" );

    *pResult = 0;

    CTelephoneManagerDlg* pMainWnd = dynamic_cast<CTelephoneManagerDlg*>(AfxGetMainWnd());
    ASSERT_VALID(pMainWnd);

    UpdateRights( pMainWnd );
    clearTabBtns();
    DisplayButtons();

    SetDlgItemText(IDC_LABEL_EDIT, _T(""));
    SetDlgItemText(IDC_NUMBER_EDIT, _T(""));

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectLinesPage::OnSetButton()
{
    FUNCTION_ENTRY( "OnSetButton" );

    TA_ASSERT(m_Summary != NULL, "m_Summary is null; Call setItaSummary first.");

    UpdateData();

    RES_TEL_DIR_ENTRY_MAP* pMap = NULL;
    TelephoneSpeedDialButtonDetails details;

    int btnPos = 0;
    if(m_isHotButton)
    {
        // if(m_Label.GetLength() > 12)
        if(m_Label.GetLength() > 10) // TD 15893
        {
            // TD14164 ++
            /*AfxMessageBox("Label length must be less than or equal to 12 characters!!");*/
            TA_Base_Bus::TransActiveMessage userMsg;
            CString number = "10";
            userMsg << number;
            userMsg.showMsgBox(IDS_UE_620006);
            // ++ TD14164
            FUNCTION_EXIT;
            return;
        }

		//validate the input number
		int len = m_Number.GetLength();
		BOOL bIsValid = TRUE;
		for (int i = 0; i < len; i++)
		{
			char temp = m_Number.GetAt(i);
			if (temp < '0' || temp > '9')
			{
				bIsValid = FALSE;
				break;
			}
		}
		if (!bIsValid)
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_620021);
			FUNCTION_EXIT;
			return;
		}
		//validate the input number

        details.SetName = TelephoneDirectLinesLibrary::SETNAME_HOTLINES;
        btnPos = m_buttonNumber - HOTLINE_BUTTONS_BASE_RESOURCE_ID;
        pMap = &m_Hotlines;
    }
    else
    {
        // if(m_Label.GetLength() > 6)
        if(m_Label.GetLength() > 10) // TD 15893
        {
            // TD14164 ++
            /*AfxMessageBox("Label length must be less than or equal to 6 characters!!");*/
            TA_Base_Bus::TransActiveMessage userMsg;
            CString number = "10";
            userMsg << number;
            userMsg.showMsgBox(IDS_UE_620006);
            // ++ TD14164
            FUNCTION_EXIT;
            return;
        }

		//validate the input number
		int len = m_Number.GetLength();
		BOOL bIsValid = TRUE;
		for (int i = 0; i < len; i++)
		{
			char temp = m_Number.GetAt(i);
			if (temp < '0' || temp > '9')
			{
				bIsValid = FALSE;
				break;
			}
		}
		if (!bIsValid)
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_620021);
			FUNCTION_EXIT;
			return;
		}
		//validate the input number
        btnPos = m_buttonNumber - TAB_BUTTONS_BASE_RESOURCE_ID;
        int selected = m_TabControl.GetCurSel();
        switch(selected)
        {
            case 0:
                details.SetName = TelephoneDirectLinesLibrary::SETNAME_USER;
                pMap = &m_userEntries;
                break;

            case 1:
                details.SetName = TelephoneDirectLinesLibrary::SETNAME_STATIONS;
                pMap = &m_stationsEntries;
                break;

            case 2:
                details.SetName = TelephoneDirectLinesLibrary::SETNAME_OCR;
                pMap = &m_OCREntries;
                break;

            case 3:
                details.SetName = TelephoneDirectLinesLibrary::SETNAME_DCR;
                pMap = &m_DCREntries;
                break;

            case 4:
                details.SetName = TelephoneDirectLinesLibrary::SETNAME_OTHERS;
                pMap = &m_otherEntries;
                break;

            case 5:
                details.SetName = TelephoneDirectLinesLibrary::SETNAME_PCP;
                pMap = &m_PCPEntries;
                break;

            default:
                pMap = &m_userEntries;
                break;
        };
    }

    details.label = m_Label.GetBuffer(m_Label.GetLength()+1);
    details.telephoneNumber = m_Number;
    details.buttonPosition = btnPos;

    try
    {
        // Where to get the resources in the sel area??
        if(TelephoneDirectLinesLibrary::setButton(m_Summary->getMySessionId(), details) == false)
        {
            TA_Base_Bus::TransActiveMessage tMsg;
            tMsg.showMsgBox(IDS_UE_620003);

            FUNCTION_EXIT;
            return;
       }
    }
    catch(TelephoneDirectLinesLibraryException& e)
    {
        std::ostringstream reason;
        reason << "DataException caught [" << e.what() << "]";
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, reason.str().c_str());

        FUNCTION_EXIT;
        return;
    }

    // Submit the Audit messages
    TA_Base_Core::NameValuePairs params;
    static_cast<TA_Base_Core::ItaAuditing*>(m_Summary)->submitAuditMessage(TA_Base_Core::TelephoneManagerAudit::TeleDirectLineConfigModified, params);

    FUNCTION_EXIT;
}


LRESULT CTelephoneConfigureDirectLinesPage::OnSpeedDialButtonModified(WPARAM pdetails, LPARAM unused)
{
    FUNCTION_ENTRY( "OnSpeedDialButtonModified" );

    TA_ASSERT(m_Summary != NULL, "m_Summary is null; Call setItaSummary first.");

    TelephoneSpeedDialButtonDetails* btnDetails = (TelephoneSpeedDialButtonDetails*) pdetails;
    map<int, TelephoneDirectoryEntryData> *pMap = NULL;
    int resourceId = 0;

    if(btnDetails->SetName == TelephoneDirectLinesLibrary::SETNAME_HOTLINES)
    {
        pMap = &m_Hotlines;
        resourceId = btnDetails->buttonPosition + HOTLINE_BUTTONS_BASE_RESOURCE_ID;
    }
    else
    {
        resourceId = btnDetails->buttonPosition + TAB_BUTTONS_BASE_RESOURCE_ID;

        if(btnDetails->SetName == TelephoneDirectLinesLibrary::SETNAME_USER)
        {
            pMap = &m_userEntries;
        }
        else if(btnDetails->SetName == TelephoneDirectLinesLibrary::SETNAME_STATIONS)
        {
            pMap = &m_stationsEntries;
        }
        else if(btnDetails->SetName == TelephoneDirectLinesLibrary::SETNAME_OCR)
        {
            pMap = &m_OCREntries;
        }
        else if(btnDetails->SetName == TelephoneDirectLinesLibrary::SETNAME_DCR)
        {
            pMap = &m_DCREntries;
        }
        else if(btnDetails->SetName == TelephoneDirectLinesLibrary::SETNAME_OTHERS)
        {
            pMap = &m_otherEntries;
        }
        else
        {
            pMap = &m_PCPEntries;
        }
    }

    if(btnDetails->label.empty() && btnDetails->telephoneNumber.empty())
    {
        if(pMap->find(resourceId) != pMap->end())
        {
            pMap->erase(resourceId);
        }
    }
    else
    {
        // Retrieve record button record from DB
        TelephoneDirectoryEntryData* entryData;
        try
        {
            CWnd *pMainWnd = AfxGetMainWnd();
            ASSERT_VALID(pMainWnd);

            entryData = TelephoneDirectoryLibrary::getEntryByNumber(btnDetails->telephoneNumber);
            if(entryData != NULL)
            {
               (*pMap)[resourceId] = *entryData;
                delete entryData;
                entryData = NULL;

                pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, (*pMap)[resourceId].fullName.c_str());
                pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER,(*pMap)[resourceId].number.c_str());
                pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, (*pMap)[resourceId].location.c_str());
                pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, (*pMap)[resourceId].directoryDataName.c_str());

            }
            else
            {
                pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, _T(""));
                pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER, _T(""));
                pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, _T(""));
                pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, _T(""));

            }
        }
        catch(TelephoneDirectoryLibraryException&)
        {
            // TD14164 ++
            /*AfxMessageBox("Error downloading information from database. Please try again!");*/
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_620007);
            // ++ TD14164
            if (btnDetails != NULL)
            {
                delete btnDetails;
                btnDetails = NULL;
            }

            FUNCTION_EXIT;
            return false;
        }
    }
    // Update label of the button if currently selected
    SetDlgItemText(resourceId, btnDetails->label.c_str());
    SetDlgItemText(IDC_LABEL_EDIT, btnDetails->label.c_str());
    SetDlgItemText(IDC_NUMBER_EDIT, btnDetails->telephoneNumber.c_str());

    if (btnDetails != NULL)
    {
        delete btnDetails;
        btnDetails = NULL;
    }

    FUNCTION_EXIT;
    return true;
}


void CTelephoneConfigureDirectLinesPage::OnHotButtonPress(UINT ctrlID)
{
    FUNCTION_ENTRY( "OnHotButtonPress" );

    m_isHotButton = true;
    m_buttonNumber = ctrlID;

    CTelephoneManagerDlg* pMainWnd = dynamic_cast<CTelephoneManagerDlg*>(AfxGetMainWnd());
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
        pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, "");
        pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER,"");
        pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, "");
        pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, "");
    }

    TelephoneSpeedDialButtonDetails btndetails =
            TelephoneDirectLinesLibrary::getButtonDetails(m_Summary->getMySessionId(), TelephoneDirectLinesLibrary::SETNAME_HOTLINES, ctrlID - HOTLINE_BUTTONS_BASE_RESOURCE_ID);

    if(btndetails.buttonPosition != 0) // There is some data
    {
        SetDlgItemText(IDC_LABEL_EDIT, btndetails.label.c_str());
        SetDlgItemText(IDC_NUMBER_EDIT, btndetails.telephoneNumber.c_str());
    }
    else
    {
        SetDlgItemText(IDC_LABEL_EDIT, _T(""));
        SetDlgItemText(IDC_NUMBER_EDIT, _T(""));
    }

    bool canConfigHotLine = pMainWnd->isFunctionPermitted(TA_Base_Bus::aca_TELEPHONE_CONFIGURE_HOT_LINE);

    CWnd *pWnd = GetDlgItem(IDC_SET_BTN);
    ASSERT_VALID(pMainWnd);
    pWnd->EnableWindow(canConfigHotLine);

    pWnd = GetDlgItem(IDC_LABEL_EDIT);
    ASSERT_VALID(pMainWnd);
    pWnd->EnableWindow(canConfigHotLine);

    pWnd = GetDlgItem(IDC_NUMBER_EDIT);
    ASSERT_VALID(pMainWnd);
    pWnd->EnableWindow(canConfigHotLine);

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectLinesPage::OnDirectButtonPress(UINT ctrlID)
{
    FUNCTION_ENTRY( "OnDirectButtonPress" );

    m_isHotButton = false;
    m_buttonNumber = ctrlID;
    string selSetName ;

    int selected = m_TabControl.GetCurSel();
    if(selected < 0)
    {
        FUNCTION_EXIT;
        return;
    }


    std::map<int, TelephoneDirectoryEntryData> *pMap;
    switch(selected)
    {
        case 0:
            pMap = &m_userEntries;
            selSetName = TelephoneDirectLinesLibrary::SETNAME_USER;
            break;

        case 1:
            pMap = &m_stationsEntries;
            selSetName = TelephoneDirectLinesLibrary::SETNAME_STATIONS;
            break;

        case 2:
            pMap = &m_OCREntries;
            selSetName = TelephoneDirectLinesLibrary::SETNAME_OCR;
            break;

        case 3:
            pMap = &m_DCREntries;
            selSetName = TelephoneDirectLinesLibrary::SETNAME_DCR;
            break;

        case 4:
            pMap = &m_otherEntries;
            selSetName = TelephoneDirectLinesLibrary::SETNAME_OTHERS;
            break;

        case 5:
            pMap = &m_PCPEntries;
            selSetName = TelephoneDirectLinesLibrary::SETNAME_PCP;
            break;

        default:
            pMap = &m_userEntries;
            selSetName = TelephoneDirectLinesLibrary::SETNAME_USER;
            break;
    };

    CTelephoneManagerDlg* pMainWnd = dynamic_cast<CTelephoneManagerDlg*>(AfxGetMainWnd());
    ASSERT_VALID(pMainWnd);

    if((*pMap).empty() == false && pMap->find(ctrlID) != pMap->end())
    {
        pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, (*pMap)[ctrlID].fullName.c_str());
        pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER, (*pMap)[ctrlID].number.c_str());
        pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, (*pMap)[ctrlID].location.c_str());
        pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, (*pMap)[ctrlID].directoryDataName.c_str());
    }
    else
    {
        pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, "");
        pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER,"");
        pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, "");
        pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, "");
    }

    TelephoneSpeedDialButtonDetails btndetails =
            TelephoneDirectLinesLibrary::getButtonDetails(m_Summary->getMySessionId(), selSetName, ctrlID - TAB_BUTTONS_BASE_RESOURCE_ID);

    if(btndetails.buttonPosition != 0) // There is some data
    {
        SetDlgItemText(IDC_LABEL_EDIT, btndetails.label.c_str());
        SetDlgItemText(IDC_NUMBER_EDIT, btndetails.telephoneNumber.c_str());
    }
    else
    {
        SetDlgItemText(IDC_LABEL_EDIT, _T(""));
        SetDlgItemText(IDC_NUMBER_EDIT, _T(""));
    }

    bool canConfigDirectLine = false;
    if(selSetName == TelephoneDirectLinesLibrary::SETNAME_USER)
    {
        canConfigDirectLine = pMainWnd->isFunctionPermitted(TA_Base_Bus::aca_TELEPHONE_CONFIGURE_USER_DIRECT_LINE);
    }
    else
    {
        canConfigDirectLine = pMainWnd->isFunctionPermitted(TA_Base_Bus::aca_TELEPHONE_CONFIGURE_SHARED_DIRECT_LINE);
    }

    CWnd *pWnd = GetDlgItem(IDC_SET_BTN);
    ASSERT_VALID(pMainWnd);
    pWnd->EnableWindow(canConfigDirectLine);

    pWnd = GetDlgItem(IDC_LABEL_EDIT);
    ASSERT_VALID(pMainWnd);
    pWnd->EnableWindow(canConfigDirectLine);

    pWnd = GetDlgItem(IDC_NUMBER_EDIT);
    ASSERT_VALID(pMainWnd);
    pWnd->EnableWindow(canConfigDirectLine);

    FUNCTION_EXIT;
}


// ---- Helper functions -----
void CTelephoneConfigureDirectLinesPage::updateLabelButtons(
            unsigned int resourceBaseIndex,
            map<int, TelephoneDirectoryEntryData>&   iButtonMap,
            vector<TelephoneSpeedDialButtonDetails>& iBtnSet)
{
    FUNCTION_ENTRY( "updateLabelButtons" );

    int resourceIndex = 0;
    TelephoneDirectoryEntryData* entryData = NULL;

	//libo++
    TelephoneDirectoryEntryDataMap entryDataMap;
	unsigned int nBntNumber =iBtnSet.size();
	if(nBntNumber <=0)
	{
		FUNCTION_EXIT;
		return;
	}
	std::string sNumberScope;
	sNumberScope = " ";
	for(unsigned int n = 0; n < nBntNumber; n++)
    {
		sNumberScope += iBtnSet[n].telephoneNumber;
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
	
    // Update the Hotline Buttons
    for(unsigned int i = 0; i < nBntNumber; i++)
    {
        resourceIndex = iBtnSet[i].buttonPosition + resourceBaseIndex;

        // Update the button labels
        SetDlgItemText(resourceIndex, iBtnSet[i].label.c_str());

		//libo++
 		TelephoneDirectoryEntryDataMap::iterator  entryDataIt = entryDataMap.find(iBtnSet[i].telephoneNumber);
		if(entryDataIt !=  entryDataMap.end())
		{
			iButtonMap[resourceIndex] = *(entryDataIt->second);
			delete entryDataIt->second;
		}
		//++libo
    }

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectLinesPage::clearTabBtns()
{
    FUNCTION_ENTRY( "clearTabBtns" );

    for(int i = 1; i <= MAX_TAB_BUTTONS; i++)
    {
        SetDlgItemText(TAB_BUTTONS_BASE_RESOURCE_ID + i, "");
    }

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectLinesPage::clearHotlinesBtns()
{
    FUNCTION_ENTRY( "clearHotlinesBtns" );

    for(int i = 1; i <= MAX_HOTLINE_BUTTONS; i++)
    {
        SetDlgItemText(HOTLINE_BUTTONS_BASE_RESOURCE_ID + i, "");
    }

    FUNCTION_EXIT;
}


BOOL CTelephoneConfigureDirectLinesPage::OnSetActive()
{
    FUNCTION_ENTRY( "OnSetActive" );

    //LIMIN TODO
    static bool isFirstTime = true;
    if ( isFirstTime )
    {
        isFirstTime = false;
        return TRUE;
    }

    CTelephoneManagerDlg* pMainWnd = dynamic_cast<CTelephoneManagerDlg*>(AfxGetMainWnd());
    ASSERT_VALID(pMainWnd);

    UpdateRights( pMainWnd );

    if(false == m_Hotlines.empty())
    {
        m_Hotlines.clear();
    }

    if(false == m_userEntries.empty())
    {
        m_userEntries.clear();
    }

    if(false == m_stationsEntries.empty())
    {
        m_stationsEntries.clear();
    }

    if(false == m_OCREntries.empty())
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
    FUNCTION_EXIT;
    return CPropertyPage::OnSetActive();
}
