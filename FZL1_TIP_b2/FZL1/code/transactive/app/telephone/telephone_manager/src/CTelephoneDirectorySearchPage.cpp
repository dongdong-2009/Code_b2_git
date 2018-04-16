/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/CTelephoneDirectorySearchPage.cpp $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * TelephoneDirectorySearchPage.cpp 
  *
  * Class implements the search functionality for the telephone manager
  * It handles the "Search" Property Page.
  */


#include "app/telephone/telephone_manager/src/StdAfx.h"
#include "app/telephone/telephone_manager/src/TelephoneApp.h"

#include "app/telephone/telephone_manager/src/CTelephoneDirectorySearchPage.h"
#include "app/telephone/telephone_manager/src/CTelephoneManagerDlg.h"
#include "app/telephone/telephone_manager/src/ItaTelephoneManagerSummary.h"
#include "app/telephone/telephone_manager/src/TelephoneManagerUserEvents.h"

#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibrary.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibraryException.h"
#include "bus/telephone/telephone_direct_lines_library/src/TelephoneDirectLinesLibrary.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;


////////////////////////////////////////////////////////////////////////////
// CTelephoneDirectorySearchPage property page

IMPLEMENT_DYNCREATE(CTelephoneDirectorySearchPage, CPropertyPage)

const string CTelephoneDirectorySearchPage::COLUMN_NAME = "Contact Name";
const int CTelephoneDirectorySearchPage::COLUMN_INDEX = 0;
const int CTelephoneDirectorySearchPage::COLUMN_OFFSET = 60;

typedef map<HTREEITEM, TelephoneDirectoryEntryData>    TEL_DIR_ENTRY_MAP;
typedef map<HTREEITEM, TelephoneDirectoryData>         TEL_DIR_MAP;

TEL_DIR_MAP::iterator searchForDirectoryByID(TEL_DIR_MAP& directoryMap, const unsigned long ID)
{
    for (TEL_DIR_MAP::iterator it = directoryMap.begin(); it != directoryMap.end(); ++it)
    {
        if(it->second.ID == ID)
            return it;
    }

    return directoryMap.end();
}

TEL_DIR_MAP::iterator searchForDirectoryByDirName(TEL_DIR_MAP& directoryMap, const std::string subDir)
{
    for (TEL_DIR_MAP::iterator it = directoryMap.begin(); it != directoryMap.end(); ++it)
    {
        if(it->second.name.compare(subDir) == 0)
            return it;
    }

    return directoryMap.end();
}

TEL_DIR_ENTRY_MAP::iterator searchForEntryByID(TEL_DIR_ENTRY_MAP& entryMap, const unsigned long ID)
{
    for (TEL_DIR_ENTRY_MAP::iterator it =  entryMap.begin(); it !=  entryMap.end(); ++it)
    {
        if(it->second.ID == ID)
            return it;
    }

    return entryMap.end();
}

TEL_DIR_ENTRY_MAP::iterator searchForEntryByName(TEL_DIR_ENTRY_MAP& entryMap, const string& fullName)
{
    for (TEL_DIR_ENTRY_MAP::iterator it =  entryMap.begin(); it !=  entryMap.end(); ++it)
    {
        if (it->second.fullName.compare(fullName) == 0)
            return it;
    }

    return entryMap.end();
}


class CTelephoneDirectorySearchPageInitialThread : public TA_Base_Core::Thread
{
public:
    CTelephoneDirectorySearchPageInitialThread( CTelephoneDirectorySearchPage& page, CTelephoneManagerDlg* pMainWnd )
        : m_page( page ),
          m_pMainWnd( pMainWnd) {};
    ~CTelephoneDirectorySearchPageInitialThread() { terminateAndWait(); };
    virtual void run()
    {
        try
        {
            HTREEITEM mainDir = m_page.m_DirectoryTree.InsertItem("All");
            HTREEITEM parentHdl, dirHdl;
            unsigned int i = 0;

            vector<TelephoneDirectoryData> directories = TelephoneDirectoryLibrary::getAllDirectories();
            for(i = 0; i < directories.size(); i++)
            {
                parentHdl = m_page.m_DirectoryTree.InsertItem(directories[i].name.c_str(), mainDir);
                m_page.m_DirectoryMap[parentHdl] = directories[i];
            }

            vector<TelephoneDirectoryEntryData> entries = TelephoneDirectoryLibrary::getAllEntries();

            for ( i = 0; i < entries.size(); i++ )
            {
                TEL_DIR_MAP::iterator iter =
                    searchForDirectoryByDirName( m_page.m_DirectoryMap, entries[i].directoryDataName);

                if(iter == m_page.m_DirectoryMap.end()) // not found => new item
                {
                    // return false;
                }
                else
                {
                    // Add item in the CTree, m_DirectoryEntryMap
                    parentHdl = iter->first;
                }

                dirHdl = m_page.m_DirectoryTree.InsertItem(entries[i].fullName.c_str(),parentHdl);
                m_page.m_DirectoryEntryMap[dirHdl] = entries[i];
            }
        }
        catch(TelephoneDirectoryLibraryException&)
        {
            /*AfxMessageBox("Error reading contacts from database!");*/
            // TD14164 ++
            TA_Base_Bus::TransActiveMessage userMsg;
            CString fieldName = "contacts";
            userMsg << fieldName;
            userMsg.showMsgBox(IDS_UE_620011);
            // ++ TD14164
        }

        CRect rect;
        CWnd* pWnd = m_page.GetDlgItem(IDC_SEARCH_LISTING);
        VERIFY(pWnd != NULL);
        pWnd->GetClientRect(rect);
        VERIFY( m_page.m_SearchList.InsertColumn( m_page.COLUMN_INDEX, m_page.COLUMN_NAME.c_str(), LVCFMT_CENTER, rect.right + m_page.COLUMN_OFFSET) != -1);        
    }
    virtual void terminate() {};
private:
    CTelephoneDirectorySearchPage& m_page;
    CTelephoneManagerDlg* m_pMainWnd;
};


CTelephoneDirectorySearchPage::CTelephoneDirectorySearchPage()
    :   CPropertyPage(CTelephoneDirectorySearchPage::IDD),
        m_SearchList(1),
        m_SortInfo(1),
        m_initialThread( NULL )
{
    FUNCTION_ENTRY( "CTelephoneDirectorySearchPage" );

    // {{AFX_DATA_INIT(CTelephoneDirectorySearchPage)
    m_SearchItem = _T("");
    // }}AFX_DATA_INIT

    // m_SearchList.SortItems(
    m_SortInfo.defineSortingSemantic(0, TA_Base_Bus::AutoSortListCtrl::BY_STRING);

    FUNCTION_EXIT;
}


CTelephoneDirectorySearchPage::~CTelephoneDirectorySearchPage()
{
    FUNCTION_ENTRY( "~CTelephoneDirectorySearchPage" );

    delete m_initialThread;
    m_initialThread = NULL;

    FUNCTION_EXIT;
}


void CTelephoneDirectorySearchPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPropertyPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CTelephoneDirectorySearchPage)
    DDX_Control(pDX, IDC_DIRECTORY, m_DirectoryTree);
    DDX_Text(pDX, IDC_SEARCH_ITEM, m_SearchItem);
    DDV_MaxChars(pDX, m_SearchItem, 20);
    DDX_Control(pDX, IDC_SEARCH_LISTING, m_SearchList);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


void CTelephoneDirectorySearchPage::setItaSummary(ItaTelephoneManagerSummary* iSummary)
{
    FUNCTION_ENTRY( "setItaSummary" );

    m_Summary = iSummary;

    FUNCTION_EXIT;
}


void CTelephoneDirectorySearchPage::UpdateRights()
{
    FUNCTION_ENTRY( "UpdateRights" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CTelephoneDirectorySearchPage, CPropertyPage)
    // {{AFX_MSG_MAP(CTelephoneDirectorySearchPage)
    ON_BN_CLICKED(IDC_SEARCH_BTN, OnSearchClicked)
    ON_NOTIFY(NM_CLICK, IDC_SEARCH_LISTING, OnClickSearchListing)
    ON_EN_CHANGE(IDC_SEARCH_ITEM, OnChangeEditSearchText)
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_DIRECTORY_UPLOAD,         OnDirectoryReload       )
    ON_MESSAGE(WM_DIRECTORY_ENTRY_ADDED ,   OnDirectoryEntryAdded   )
    ON_MESSAGE(WM_DIRECTORY_ENTRY_MODIFIED, OnDirectoryEntryModified)
    ON_MESSAGE(WM_DIRECTORY_ENTRY_DELETED,  OnDirectoryEntryDeleted )
    ON_MESSAGE(WM_DIRECTORY_ADDED ,         OnDirectoryAdded        )
    ON_MESSAGE(WM_DIRECTORY_MODIFIED,       OnDirectoryModified     )
    ON_MESSAGE(WM_DIRECTORY_DELETED,        OnDirectoryDeleted      )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTelephoneDirectorySearchPage message handlers
LRESULT CTelephoneDirectorySearchPage::OnDirectoryReload(WPARAM param1, LPARAM param2)
{
    FUNCTION_ENTRY( "OnDirectoryReload" );

    m_DirectoryMap.clear();
    m_DirectoryEntryMap.clear();
    m_DirectoryTree.DeleteAllItems();

    TA_ASSERT(m_Summary != NULL, "NULL CtaTelephoneManagerSummary");
    try
    {
        HTREEITEM mainDir = m_DirectoryTree.InsertItem("All");
        HTREEITEM parentHdl, dirHdl;
        unsigned int i = 0;

        vector<TelephoneDirectoryData> directories = TelephoneDirectoryLibrary::getAllDirectories();
        for(i = 0; i < directories.size(); i++)
        {
            parentHdl = m_DirectoryTree.InsertItem(directories[i].name.c_str(), mainDir);
            m_DirectoryMap[parentHdl] = directories[i];
        }

        vector<TelephoneDirectoryEntryData> entries = TelephoneDirectoryLibrary::getAllEntries();

        for(i = 0; i < entries.size(); i++)
        {
            TEL_DIR_MAP::iterator iter =
                searchForDirectoryByDirName(m_DirectoryMap, entries[i].directoryDataName);

            if(iter == m_DirectoryMap.end()) // not found => new item
            {
                FUNCTION_EXIT;
                return false;
            }
            else
            {
                // Add item in the CTree, m_DirectoryEntryMap
                parentHdl = iter->first;
            }

            dirHdl = m_DirectoryTree.InsertItem(entries[i].fullName.c_str(),parentHdl);
            m_DirectoryEntryMap[dirHdl] = entries[i];
        }
    }
    catch(TelephoneDirectoryLibraryException&)
    {
        /*AfxMessageBox("Error reading contacts from database!");*/
        // TD14164 ++
        TA_Base_Bus::TransActiveMessage userMsg;
        CString fieldName = "contacts";
        userMsg << fieldName;
        userMsg.showMsgBox(IDS_UE_620011);
        // ++ TD14164
    }

    FUNCTION_EXIT;
    return (LRESULT)0L;
}


LRESULT CTelephoneDirectorySearchPage::OnDirectoryEntryAdded(WPARAM param1, LPARAM param2)
{
    FUNCTION_ENTRY( "OnDirectoryEntryAdded" );

    TA_ASSERT(param1 != NULL, "OnDirectoryEntryAdded: param1 is null.");

    TelephoneDirectoryEntryData* newEntry = (TelephoneDirectoryEntryData*)(param1);

    TEL_DIR_MAP::iterator iter = searchForDirectoryByDirName(m_DirectoryMap, newEntry->directoryDataName);

    if(iter != m_DirectoryMap.end()) // found
    {
        HTREEITEM parentHdl = iter->first;

        HTREEITEM dirHdl = m_DirectoryTree.InsertItem(newEntry->fullName.c_str(),parentHdl);
        m_DirectoryEntryMap[dirHdl] = *newEntry;
    }
    else
    {
        // TD14164 ++
        /*string errMsg = "Directory: "
            + newEntry->directoryDataName
            + " cannot be found! Contact cannot be added.";
        AfxMessageBox(errMsg.c_str());*/
        TA_Base_Bus::TransActiveMessage userMsg;
        std::string fieldName = newEntry->directoryDataName;
        userMsg << fieldName.c_str();
        userMsg.showMsgBox(IDS_UE_620012);
        // ++ TD14164
    }

    delete newEntry;

    FUNCTION_EXIT;
    return 0L;
}


LRESULT CTelephoneDirectorySearchPage::OnDirectoryEntryModified(WPARAM param1, LPARAM param2)
{
    FUNCTION_ENTRY( "OnDirectoryEntryModified" );

    TA_ASSERT(param1 != NULL, "OnDirectoryEntryModified: param1 is null.");

    TelephoneDirectoryEntryData* pDirectoryEntry = reinterpret_cast<TelephoneDirectoryEntryData*>(param1);

    // search m_DirectoryEntryMap for the handler of the old entry
    TEL_DIR_ENTRY_MAP::iterator entryIt =
        searchForEntryByID(m_DirectoryEntryMap, pDirectoryEntry->ID);

    if(entryIt != m_DirectoryEntryMap.end())
    {
        m_DirectoryEntryMap[entryIt->first] = *pDirectoryEntry;
        VERIFY(m_DirectoryTree.SetItemText(entryIt->first, pDirectoryEntry->fullName.c_str()));
    }
    else
    {
        // TD14164 ++
        /*string errMsg = "Cannot find contact with ID: " + pDirectoryEntry->ID;
        AfxMessageBox(errMsg.c_str());*/
        TA_Base_Bus::TransActiveMessage userMsg;
        std::ostringstream str;
        str << pDirectoryEntry->ID;
        std::string fieldName = str.str();;
        userMsg << fieldName.c_str();
        userMsg.showMsgBox(IDS_UE_620013);
        // ++ TD14164
    }

    delete pDirectoryEntry;

    FUNCTION_EXIT;
    return 0L;
}


LRESULT CTelephoneDirectorySearchPage::OnDirectoryEntryDeleted(WPARAM param1, LPARAM param2)
{
    FUNCTION_ENTRY( "OnDirectoryEntryDeleted" );

    TA_ASSERT(param1 != NULL, "OnDirectoryEntryModified: param1 is null.");

    TelephoneDirectoryEntryData* delEntry = reinterpret_cast<TelephoneDirectoryEntryData*>(param1);

    // search m_DirectoryEntryMap for the handler of the old entry
    TEL_DIR_ENTRY_MAP::iterator entryIter =
        searchForEntryByID(m_DirectoryEntryMap, delEntry->ID);

    if(entryIter != m_DirectoryEntryMap.end())
    {
        VERIFY(m_DirectoryTree.DeleteItem(entryIter->first));
        m_DirectoryEntryMap.erase(entryIter->first);
    }
    else
    {
        // TD14164 ++
        /*string errMsg = "Contact: "
                             + delEntry->fullName
                             + "with number "
                             + delEntry->number
                             + " cannot be found";
        AfxMessageBox(errMsg.c_str());*/
        TA_Base_Bus::TransActiveMessage userMsg;
        std::string fieldName = delEntry->fullName;
        std::string number = delEntry->number;
        userMsg << fieldName.c_str();
        userMsg << number.c_str();
        userMsg.showMsgBox(IDS_UE_620014);
        // ++ TD14164
    }

    delete delEntry;

    FUNCTION_EXIT;
    return 0L;
}


LRESULT CTelephoneDirectorySearchPage::OnDirectoryAdded(WPARAM param1, LPARAM param2)
{
    FUNCTION_ENTRY( "OnDirectoryAdded" );

    TA_ASSERT(param1 != NULL, "OnDirectoryEntryModified: param1 is null.");

    TelephoneDirectoryData* directoryData = reinterpret_cast<TelephoneDirectoryData*>(param1);

    TEL_DIR_MAP::iterator iter =
        searchForDirectoryByID(m_DirectoryMap, directoryData->ID);

    if(iter == m_DirectoryMap.end()) // not found
    {
        // Add item in the CTree, m_DirectoryMap
        HTREEITEM treeHdl = m_DirectoryTree.InsertItem(directoryData->name.c_str(), m_DirectoryTree.GetRootItem());
        m_DirectoryMap[treeHdl] = *directoryData;
    }
    else
    {
        // TD14164 ++
        /*string errMsg = "Directory " + directoryData->name + " already exist";
        AfxMessageBox(errMsg.c_str());*/
        TA_Base_Bus::TransActiveMessage userMsg;
        std::string fieldName = directoryData->name;
        userMsg << fieldName.c_str();
        userMsg.showMsgBox(IDS_UE_620015);
        // ++ TD14164

    }

    delete directoryData;

    FUNCTION_EXIT;
    return 0L;
}


LRESULT CTelephoneDirectorySearchPage::OnDirectoryModified(WPARAM param1, LPARAM param2)
{
    FUNCTION_ENTRY( "OnDirectoryModified" );

    TA_ASSERT(param1 != NULL, "OnDirectoryEntryModified: param1 is null.");

    // Do we need to update the all entry map that falls under the directory as well
    TelephoneDirectoryData* newDir = reinterpret_cast<TelephoneDirectoryData*>(param1);

    TEL_DIR_MAP::iterator iter =
        searchForDirectoryByID(m_DirectoryMap, newDir->ID);

    if(iter != m_DirectoryMap.end()) // found
    {
        HTREEITEM entryHdl = NULL;
        HTREEITEM dirHdl = iter->first;

        // Upate the m_DirectoryTree and m_DirectoryMap
        VERIFY(m_DirectoryTree.SetItemText(dirHdl, newDir->name.c_str()));

        iter->second = *newDir;

        // Update the m_DirectoryEntryMap as well
        if(m_DirectoryTree.ItemHasChildren(dirHdl))
        {
            entryHdl = m_DirectoryTree.GetChildItem(dirHdl);
            while(entryHdl != NULL)
            {
                m_DirectoryEntryMap[entryHdl].directoryDataName = newDir->name;
                entryHdl = m_DirectoryTree.GetNextSiblingItem(entryHdl);
            }
        }
    }
    else
    {
        /*string errMsg = "Cannot find Directory";
        AfxMessageBox(errMsg.c_str());*/
        // TD14164 ++
        TA_Base_Bus::TransActiveMessage userMsg;
        CString fieldName = "Directory";
        userMsg << fieldName;
        userMsg.showMsgBox(IDS_UE_620016);
        // ++ TD14164
    }

    delete newDir;

    FUNCTION_EXIT;
    return 0L;
}


LRESULT CTelephoneDirectorySearchPage::OnDirectoryDeleted(WPARAM param1, LPARAM param2)
{
    FUNCTION_ENTRY( "OnDirectoryDeleted" );

    TA_ASSERT(param1 != NULL, "OnDirectoryDeleted: param1 is null.");

    TelephoneDirectoryData* delDir = (TelephoneDirectoryData*)(param1);


    TEL_DIR_MAP::iterator iter =
        searchForDirectoryByID(m_DirectoryMap, delDir->ID);

    if(iter != m_DirectoryMap.end()) // found
    {
        // Erase CTree, m_DirectoryMap
        // Assuming here that DeleteItem deletes all items under the root?
        if(m_DirectoryTree.DeleteItem(iter->first) == false)
        {
            // TD14164 ++
            /*string errMsg = "Directory: "
                                 + delDir->name
                                 + " cannot be deleted";
            AfxMessageBox(errMsg.c_str());*/
            TA_Base_Bus::TransActiveMessage userMsg;
            std::string fieldName = delDir->name;
            userMsg << fieldName.c_str();
            userMsg.showMsgBox(IDS_UE_620017);
            // ++ TD14164
            FUNCTION_EXIT;
            return false;
        }

        HTREEITEM entryHdl = NULL;
        HTREEITEM dirHdl = iter->first;
        m_DirectoryMap.erase(dirHdl);

        // Erase m_DirectoryEntryMap??
        if(m_DirectoryTree.ItemHasChildren(dirHdl))
        {
            entryHdl = m_DirectoryTree.GetChildItem(dirHdl);
            while(entryHdl != NULL)
            {
                m_DirectoryEntryMap.erase(entryHdl);
                entryHdl = m_DirectoryTree.GetNextSiblingItem(entryHdl);
            }
        }
    }

    delete delDir;

    FUNCTION_EXIT;
    return 0L;
}


BOOL CTelephoneDirectorySearchPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    TA_ASSERT(m_Summary != NULL, "m_Summary is Null; Call setItaSummary first.");

    CPropertyPage::OnInitDialog();

    CTelephoneManagerDlg* pMainWnd = dynamic_cast<CTelephoneManagerDlg*>(AfxGetMainWnd());
    ASSERT_VALID(pMainWnd);

    m_initialThread = new CTelephoneDirectorySearchPageInitialThread( *this, pMainWnd );
    m_initialThread->start();

    FUNCTION_EXIT;
    return TRUE;
}


void CTelephoneDirectorySearchPage::OnSearchClicked()
{
    FUNCTION_ENTRY( "OnSearchClicked" );

    TA_ASSERT(m_Summary != NULL, "m_Summary is Null; Call setItaSummary first.");

    string sessionId = m_Summary->getMySessionId();

    CWnd *pMainWnd = AfxGetMainWnd();
    ASSERT_VALID(pMainWnd);
    pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, "");
    pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER, "");
    pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, "");
    pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, "");
    // Check if tree is selected - if yes, get directory selected
    // if not, dir = "All"

    string dirName;

    HTREEITEM selHdl = m_DirectoryTree.GetSelectedItem();
    if(m_DirectoryMap.find(selHdl) == m_DirectoryMap.end())
    {
        dirName = "";
    }

    else
        dirName = m_DirectoryMap[selHdl].name;

    UpdateData();

    string toSearch = m_SearchItem;

    CWaitCursor cursor;
    vector<TelephoneDirectoryEntryData> foundList =
        TelephoneDirectoryLibrary::searchDirectory(dirName, toSearch);

    m_SearchList.DeleteAllItems();
    for(unsigned int i = 0; i < foundList.size(); i++)
    {
        int nIndex = m_SearchList.InsertItem(i, foundList[i].fullName.c_str());
    }

    FUNCTION_EXIT;
}


void CTelephoneDirectorySearchPage::OnClickSearchListing(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnClickSearchListing" );

    // Display on the main telephoneGUI's "selected area"
    *pResult = 0;

    UpdateData();

    POSITION pos = m_SearchList.GetFirstSelectedItemPosition();
    if (pos != NULL)
    {
        int ipos = (int)--pos;
        CString selected = m_SearchList.GetItemText(ipos, 0);
        string fullName = selected;

        CWnd *pMainWnd = AfxGetMainWnd();
        ASSERT_VALID(pMainWnd);

        TEL_DIR_ENTRY_MAP::iterator iter = searchForEntryByName(m_DirectoryEntryMap, fullName);
        if(iter == m_DirectoryEntryMap.end())
        {
            m_SearchList.DeleteAllItems();
            pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, "");
            pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER, "");
            pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, "");
            pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, "");
            // AfxMessageBox("Error find selected record");
            FUNCTION_EXIT;
            return;
        }


        TelephoneDirectoryEntryData* pEntry = &(iter->second);

        CString val = pEntry->directoryDataName.c_str();
        pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, fullName.c_str());
        pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER, pEntry->number.c_str());
        pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, pEntry->directoryDataName.c_str());
        pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, pEntry->location.c_str());
    }

    FUNCTION_EXIT;
}


LRESULT CTelephoneDirectorySearchPage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "WindowProc" );

    TA_Base_Bus::AutoSortListCtrl::WindowProc(message, wParam, lParam, m_SearchList, m_SortInfo);

    FUNCTION_EXIT;
    return CPropertyPage::WindowProc(message, wParam, lParam);
}


void CTelephoneDirectorySearchPage::OnChangeEditSearchText()
{
    FUNCTION_ENTRY( "OnChangeEditSearchText" );

    UpdateData();
    GetDlgItem(IDC_SEARCH_BTN)->EnableWindow(m_SearchItem.GetLength() > 0);

    FUNCTION_EXIT;
}


//++for keyboard up&down operation
BOOL CTelephoneDirectorySearchPage::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_UP)
		{
			UpdateData();
			
			POSITION pos = m_SearchList.GetFirstSelectedItemPosition();
			if (pos != NULL)
			{
				int ipos = (int)--pos;
				if (ipos != 0)
				{
					ipos = ipos - 1;
				}
	
				CString selected = m_SearchList.GetItemText(ipos, 0);
				string fullName = selected;
				
				CWnd *pMainWnd = AfxGetMainWnd();
				ASSERT_VALID(pMainWnd);
				
				TEL_DIR_ENTRY_MAP::iterator iter = searchForEntryByName(m_DirectoryEntryMap, fullName);				
				
				TelephoneDirectoryEntryData* pEntry = &(iter->second);
				
				CString val = pEntry->directoryDataName.c_str();
				pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, fullName.c_str());
				pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER, pEntry->number.c_str());
				pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, pEntry->directoryDataName.c_str());
				pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, pEntry->location.c_str());
			}
		}
		else if (pMsg->wParam == VK_DOWN)
		{
			UpdateData();
			
			POSITION pos = m_SearchList.GetFirstSelectedItemPosition();
			if (pos != NULL)
			{
				int listSize = m_SearchList.GetItemCount();

				int ipos = (int)--pos;
				if (ipos < listSize - 1 )
				{
					ipos = ipos + 1;
				}
				else
				{
					ipos = listSize - 1;
				}
				
				CString selected = m_SearchList.GetItemText(ipos, 0);

				string fullName = selected;
				
				CWnd *pMainWnd = AfxGetMainWnd();
				ASSERT_VALID(pMainWnd);
				
				TEL_DIR_ENTRY_MAP::iterator iter = searchForEntryByName(m_DirectoryEntryMap, fullName);				

				TelephoneDirectoryEntryData* pEntry = &(iter->second);
				
				CString val = pEntry->directoryDataName.c_str();
				pMainWnd->SetDlgItemText(IDC_SELECTED_NAME, fullName.c_str());
				pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER, pEntry->number.c_str());
				pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, pEntry->directoryDataName.c_str());
				pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, pEntry->location.c_str());
			}
		}
	}
	return CPropertyPage::PreTranslateMessage(pMsg);
}
//++for keyboard up&down operation