/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_manager/src/CTelephoneConfigureDirectoryPage.cpp $
  * @author:  Catherine Loh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  * Class implements the search functionality for the telephone manager
  * It handles the "Configure" Property Page.
  */

#include "app/telephone/telephone_manager/src/stdafx.h"
#include "app/telephone/telephone_manager/src/TelephoneApp.h"
#include "app/telephone/telephone_manager/src/CTelephoneConfigureDirectoryPage.h"
#include "app/telephone/telephone_manager/src/TelephoneManagerUserEvents.h"
#include "app/telephone/telephone_manager/src/CTelephoneManagerDlg.h"
#include "app/telephone/telephone_manager/src/ItaTelephoneManagerSummary.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibrary.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibraryException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/message/types/TelephoneManagerAudit_MessageTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

static const std::string ALL("All");
/////////////////////////////////////////////////////////////////////////////
// CTelephoneConfigureDirectoryPage property page


class CTelephoneConfigureDirectoryPageHelperInitialThread : public TA_Base_Core::Thread
{
public:
    CTelephoneConfigureDirectoryPageHelperInitialThread( CTelephoneConfigureDirectoryPage& page, CTelephoneManagerDlg* pMainWnd )
        : m_page( page ),
          m_pMainWnd( pMainWnd ) {};
    ~CTelephoneConfigureDirectoryPageHelperInitialThread() { terminateAndWait(); };
    virtual void run()
    {
        m_page.LoadData();
        m_page.UpdateRights( m_pMainWnd );
    };
    virtual void terminate() {};
private:
    CTelephoneConfigureDirectoryPage& m_page;
    CTelephoneManagerDlg* m_pMainWnd;
};


IMPLEMENT_DYNCREATE(CTelephoneConfigureDirectoryPage, CPropertyPage)

typedef map<HTREEITEM, TelephoneDirectoryEntryData>    TEL_DIR_ENTRY_MAP;
typedef map<HTREEITEM, TelephoneDirectoryData>         TEL_DIR_MAP;

TEL_DIR_MAP::iterator searchForDirectory(TEL_DIR_MAP& i_map, const unsigned long& searchItem)
{
    TEL_DIR_MAP::iterator iter = i_map.begin();
    while(iter != i_map.end())
    {
        if((*iter).second.ID == searchItem)
        {
            return iter;
        }
        iter++;
    }

    return i_map.end();
}

TEL_DIR_MAP::iterator searchForDirectoryByDirName(TEL_DIR_MAP& i_map, const std::string& name)
{
    TEL_DIR_MAP::iterator iter = i_map.begin();
    while(iter != i_map.end())
    {
        if((*iter).second.name.compare(name) == 0)
        {
            return iter;
        }
        iter++;
    }

    return i_map.end();
}

TEL_DIR_ENTRY_MAP::iterator searchForEntry(TEL_DIR_ENTRY_MAP& i_map, const unsigned long& searchItem)
{
    TEL_DIR_ENTRY_MAP::iterator iter =  i_map.begin();
    while(iter !=  i_map.end())
    {
        if((*iter).second.ID == searchItem)
        {
            return iter;
        }
        iter++;
    }

    return i_map.end();
}

TEL_DIR_ENTRY_MAP::iterator searchForEntryByNumber(TEL_DIR_ENTRY_MAP& i_map, const std::string& number)
{
    TEL_DIR_ENTRY_MAP::iterator iter =  i_map.begin();
    while(iter !=  i_map.end())
    {
        if((*iter).second.number == number)
        {
            return iter;
        }
        iter++;
    }

    return i_map.end();
}

CTelephoneConfigureDirectoryPage::CTelephoneConfigureDirectoryPage()
    : CPropertyPage(CTelephoneConfigureDirectoryPage::IDD),
      m_initialThread( NULL )
{
    FUNCTION_ENTRY( "CTelephoneConfigureDirectoryPage" );

    // {{AFX_DATA_INIT(CTelephoneConfigureDirectoryPage)

    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CTelephoneConfigureDirectoryPage::~CTelephoneConfigureDirectoryPage()
{
    FUNCTION_ENTRY( "~CTelephoneConfigureDirectoryPage" );

    delete m_initialThread;
    m_initialThread = NULL;

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectoryPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPropertyPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CTelephoneConfigureDirectoryPage)
    DDX_Control(pDX, IDC_TREE, m_DirectoryTree);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectoryPage::UpdateRights( CTelephoneManagerDlg* pMainWnd )
{
    FUNCTION_ENTRY( "UpdateRights" );

    m_bIsPermitted = pMainWnd->isFunctionPermitted(TA_Base_Bus::aca_TELEPHONE_CONFIGURE_DIRECTORY);

    GetDlgItem(IDC_NEW_BTN)->EnableWindow(m_bIsPermitted);
    GetDlgItem(IDC_UPDATE_BTN)->EnableWindow(m_bIsPermitted);
    GetDlgItem(IDC_DELETE_BTN)->EnableWindow(m_bIsPermitted);

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CTelephoneConfigureDirectoryPage, CPropertyPage)
    // {{AFX_MSG_MAP(CTelephoneConfigureDirectoryPage)
    ON_BN_CLICKED(IDC_NEW_BTN, OnNewClicked)
    ON_BN_CLICKED(IDC_UPDATE_BTN, OnUpdateClicked)
    ON_BN_CLICKED(IDC_DELETE_BTN, OnDeleteClicked)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
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
// CTelephoneConfigureDirectoryPage message handlers
LRESULT CTelephoneConfigureDirectoryPage::OnDirectoryReload(WPARAM unused1, LPARAM unused2)
{
    FUNCTION_ENTRY( "OnDirectoryReload" );

    // Clear all internal structures
    m_DirectoryTree.DeleteAllItems();
    m_DirectoryEntryMap.clear();
    m_DirectoryMap.clear();

    LoadData();

    FUNCTION_EXIT;
    return 0L;
}


LRESULT CTelephoneConfigureDirectoryPage::OnDirectoryEntryAdded(WPARAM pdirectoryEntryData, LPARAM unused)
{
    FUNCTION_ENTRY( "OnDirectoryEntryAdded" );

    TelephoneDirectoryEntryData *pEntryData = reinterpret_cast <TelephoneDirectoryEntryData*>(pdirectoryEntryData);
    if (pEntryData == NULL)
    {
        FUNCTION_EXIT;
        return 0L;
    }

    TEL_DIR_MAP::iterator iter = searchForDirectoryByDirName(m_DirectoryMap, pEntryData->directoryDataName);
    HTREEITEM treeHdl;

    if(iter != m_DirectoryMap.end())
    {
        HTREEITEM parentHdl = iter->first;
        treeHdl = m_DirectoryTree.InsertItem(pEntryData->fullName.c_str(), parentHdl);
        m_DirectoryTree.Expand(parentHdl, TVE_EXPAND);
    }

    m_DirectoryEntryMap[treeHdl] = *pEntryData;

    delete pEntryData;
    pEntryData = NULL;

    FUNCTION_EXIT;
    return 0L;
}


LRESULT CTelephoneConfigureDirectoryPage::OnDirectoryEntryModified(WPARAM pnewDirectoryEntryData, LPARAM unused)
{
    FUNCTION_ENTRY( "OnDirectoryEntryModified" );

    TelephoneDirectoryEntryData *pNewEntryData = reinterpret_cast <TelephoneDirectoryEntryData*>(pnewDirectoryEntryData);

    if(pNewEntryData != NULL)
    {
        // search m_DirectoryEntryMap for the handler of the old entry
        TEL_DIR_ENTRY_MAP::iterator entryIter =
            searchForEntry(m_DirectoryEntryMap, pNewEntryData->ID);

        if(entryIter != m_DirectoryEntryMap.end())
        {
            m_DirectoryEntryMap[(*entryIter).first] = *pNewEntryData;
            m_DirectoryTree.SetItemText((*entryIter).first, pNewEntryData->fullName.c_str());

            m_DirectoryTree.UpdateData();
            m_DirectoryTree.RedrawWindow();
        }

        delete pNewEntryData;
        pNewEntryData = NULL;
    }
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OnDirectoryEntryModified(). Entry data pointer is NULL");
    }

    FUNCTION_EXIT;
    return 0L;
}


LRESULT CTelephoneConfigureDirectoryPage::OnDirectoryEntryDeleted(WPARAM pdirectoryEntryData, LPARAM unused)
{
    FUNCTION_ENTRY( "OnDirectoryEntryDeleted" );

    TelephoneDirectoryEntryData *pEntryData = reinterpret_cast <TelephoneDirectoryEntryData*>(pdirectoryEntryData);

    if(pEntryData != NULL)
    {
        // search m_DirectoryEntryMap for the handler of the old entry
        TEL_DIR_ENTRY_MAP::iterator entryIter =
            searchForEntry(m_DirectoryEntryMap, pEntryData->ID);

        if(entryIter != m_DirectoryEntryMap.end())
        {
            HTREEITEM item = (*entryIter).first;

            if(m_DirectoryTree.DeleteItem((*entryIter).first))
            {
                m_DirectoryEntryMap.erase((*entryIter).first);

                FUNCTION_EXIT;
                return 0L;
            }

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OnDirectoryEntryDeleted(). Couldn't locate %ld in CTreecCtrl.", pEntryData->ID);
        }

        delete pEntryData;
        pEntryData = NULL;
    }
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OnDirectoryEntryDeleted(). Entry data pointer is NULL");
    }

    FUNCTION_EXIT;
    return 0L;
}


LRESULT CTelephoneConfigureDirectoryPage::OnDirectoryAdded(WPARAM pdirectoryData, LPARAM unused)
{
    FUNCTION_ENTRY( "OnDirectoryAdded" );

    TelephoneDirectoryData *pDirectoryData = reinterpret_cast <TelephoneDirectoryData*>(pdirectoryData);

    if(pDirectoryData != NULL)
    {

        // Bugzilla Bug #426
        // Change from searchForDirectory to searchForDirectoryByDirName
        TEL_DIR_MAP::iterator iter = searchForDirectoryByDirName(m_DirectoryMap, pDirectoryData->name);

        if(iter == m_DirectoryMap.end()) // not found
        {
            HTREEITEM treeHdl = m_DirectoryTree.GetRootItem();
            m_DirectoryTree.GetNextVisibleItem(treeHdl);
            treeHdl = m_DirectoryTree.InsertItem(pDirectoryData->name.c_str(), treeHdl);
            m_DirectoryMap[treeHdl].name = pDirectoryData->name;
            m_DirectoryMap[treeHdl].ID = pDirectoryData->ID;
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OnDirectoryAdded(). Subdirectory %s existed.", pDirectoryData->name.c_str()); //TD14337 yezh++
        }

        delete pDirectoryData;
        pDirectoryData = NULL;
    }

    FUNCTION_EXIT;
    return 0L;
}


LRESULT CTelephoneConfigureDirectoryPage::OnDirectoryModified(WPARAM pnewDirectoryData, LPARAM unused)
{
    FUNCTION_ENTRY( "OnDirectoryModified" );

    TelephoneDirectoryData *pNewDirectoryData = reinterpret_cast <TelephoneDirectoryData*>(pnewDirectoryData);
    if(pNewDirectoryData != NULL)
    {
        // search m_DirectoryEntryMap for the handler of the old entry
        TEL_DIR_MAP::iterator entryIter =
            searchForDirectory(m_DirectoryMap, pNewDirectoryData->ID);

        if(entryIter != m_DirectoryMap.end())
        {
            m_DirectoryMap[(*entryIter).first] = *pNewDirectoryData;
            if(m_DirectoryTree.SetItemText((*entryIter).first, pNewDirectoryData->name.c_str()))
            {
                FUNCTION_EXIT;
                return 0L;
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OnDirectoryModified(). Subdirectory %s couldn't be found.", pNewDirectoryData->name.c_str());
        }

        delete pNewDirectoryData;
        pNewDirectoryData = NULL;
    }

    FUNCTION_EXIT;
    return 0L;
}


LRESULT CTelephoneConfigureDirectoryPage::OnDirectoryDeleted(WPARAM pdirectoryData, LPARAM unused)
{
    FUNCTION_ENTRY( "OnDirectoryDeleted" );

    TelephoneDirectoryData *pDirectoryData = reinterpret_cast <TelephoneDirectoryData*>(pdirectoryData);
    if(pDirectoryData != NULL)
    {
        TEL_DIR_MAP::iterator iter =
            searchForDirectory(m_DirectoryMap, pDirectoryData->ID);

        if(iter != m_DirectoryMap.end()) // found
        {
            // Erase CTree, m_DirectoryMap
            // Assuming here that DeleteItem deletes all items under the root?
            HTREEITEM entryHdl = NULL;
            HTREEITEM dirHdl = (*iter).first;

            if(m_DirectoryTree.ItemHasChildren(dirHdl))
            {
                entryHdl = m_DirectoryTree.GetChildItem(dirHdl);
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Tried to delete a subdirectory %s that was not empty.",  pDirectoryData->name.c_str());
            }

            if(m_DirectoryTree.DeleteItem((*iter).first) == false)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OnDirectoryDeleted(). Subdirectory %s could not be found.", pDirectoryData->name.c_str());
            }
            else
            {
                m_DirectoryMap.erase(dirHdl);
            }
        }

        delete pDirectoryData;
        pDirectoryData = NULL;
    }

    FUNCTION_EXIT;
    return 0L;
}


void CTelephoneConfigureDirectoryPage::OnNewClicked()
{
    FUNCTION_ENTRY( "OnNewClicked" );

    CWnd *pMainWnd = AfxGetMainWnd();

    CString strDlg;

    HTREEITEM treeItem = m_DirectoryTree.GetSelectedItem();
    HTREEITEM mainDir = m_DirectoryTree.GetRootItem();
    TA_Base_Core::NameValuePairs params;
    pMainWnd->GetDlgItemText(IDC_SELECTED_DIRECTORY, strDlg);

    if(treeItem == mainDir)
    {
        TelephoneDirectoryData entryDirectory;
        if(strDlg.IsEmpty())
        {
            // TD14164 ++
            /*AfxMessageBox("Please specify a directory for your contact to be added to");*/
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_620008);
            // ++ TD14164
            FUNCTION_EXIT;
            return;
        }

        entryDirectory.name = strDlg;
        entryDirectory.ID = 0;

        if(searchForDirectoryByDirName(m_DirectoryMap, entryDirectory.name) != m_DirectoryMap.end())
        {
            // TD14164 ++
            /*AfxMessageBox("Telephone Directory already exist.");*/
            TA_Base_Bus::TransActiveMessage userMsg;
            CString fieldName = "Directory";
            userMsg << fieldName;
            userMsg.showMsgBox(IDS_UE_620009);
            // ++ TD14164
            FUNCTION_EXIT;
            return;
        }

        try
        {
            if(TelephoneDirectoryLibrary::addDirectory(entryDirectory))
            {
                params["contactName"] = entryDirectory.name;
                m_pTransactiveInterfaces->submitAuditMessage(TA_Base_Core::TelephoneManagerAudit::TeleContactAdded, params);
            }
            else
            {
                TA_Base_Bus::TransActiveMessage tMsg;
                tMsg.showMsgBox(IDS_UE_620002);

                FUNCTION_EXIT;
                return;
            }
        }
        catch(TelephoneDirectoryLibraryException& e)
        {
            std::ostringstream reason;
            reason << "TelephoneDirectoryLibraryException caught [" << e.what() << "]";
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, reason.str().c_str());

            FUNCTION_EXIT;
            return;
        }
    }
    else
    {
        TelephoneDirectoryEntryData entryData;
        if(strDlg.IsEmpty())
        {
            // TD14164 ++
            /*AfxMessageBox("Please specify a directory for your contact to be added to");*/
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_620008);
            // ++ TD14164
            FUNCTION_EXIT;
            return;
        }

        entryData.directoryDataName = strDlg;

        pMainWnd->GetDlgItemText(IDC_SELECTED_NAME, strDlg);
        entryData.fullName = strDlg;

        pMainWnd->GetDlgItemText(IDC_SELECTED_LOCATION, strDlg);
        entryData.location = strDlg;

        pMainWnd->GetDlgItemText(IDC_SELECTED_NUMBER, strDlg);
        entryData.number = strDlg;

        if(searchForEntryByNumber(m_DirectoryEntryMap, entryData.number) != m_DirectoryEntryMap.end())
        {
            // TD14164 ++
            /*AfxMessageBox("Telephone number already existed.");*/
            TA_Base_Bus::TransActiveMessage userMsg;
            CString fieldName = "number";
            userMsg << fieldName;
            userMsg.showMsgBox(IDS_UE_620009);
            // ++ TD14164
            FUNCTION_EXIT;
            return;
        }

        try
        {
            if(TelephoneDirectoryLibrary::addEntry(entryData))
            {
                params["contactName"] = entryData.fullName;
                m_pTransactiveInterfaces->submitAuditMessage(TA_Base_Core::TelephoneManagerAudit::TeleSubdirectoryAdded, params);
            }
            else
            {
                TA_Base_Bus::TransActiveMessage tMsg;
                tMsg.showMsgBox(IDS_UE_620002);

                FUNCTION_EXIT;
                return;
            }
        }
        catch(TelephoneDirectoryLibraryException& e)
        {
            std::ostringstream reason;
            reason << "TelephoneDirectoryLibraryException caught [" << e.what() << "]";
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, reason.str().c_str());

            FUNCTION_EXIT;
            return;
        }

    }

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectoryPage::OnUpdateClicked()
{
    FUNCTION_ENTRY( "OnUpdateClicked" );

    CWnd *pMainWnd = AfxGetMainWnd();

    HTREEITEM selectedItem = m_DirectoryTree.GetSelectedItem();

    CString dlgStr;
    pMainWnd->GetDlgItemText(IDC_SELECTED_DIRECTORY, dlgStr);

    TA_Base_Core::NameValuePairs params;

    if(m_DirectoryMap.find(selectedItem) != m_DirectoryMap.end())
    {
        if(searchForDirectoryByDirName(m_DirectoryMap, (LPCTSTR)dlgStr) != m_DirectoryMap.end())
        {
            // TD14164 ++
            /*AfxMessageBox(_T("Telephone Directory already exist."), MB_ICONEXCLAMATION );*/
            TA_Base_Bus::TransActiveMessage userMsg;
            CString fieldName = "Directory";
            userMsg << fieldName;
            userMsg.showMsgBox(IDS_UE_620009);
            // ++ TD14164
            FUNCTION_EXIT;
            return;
        }
        // TD 15646
//        if(m_DirectoryTree.ItemHasChildren(selectedItem))
//        {
//            AfxMessageBox(_T("Modification cannot be carried out. Directory contains existing record(s)."));
//            return;
//        }
        // TD 15646
        m_DirectoryMap[selectedItem].name = dlgStr;
        TelephoneDirectoryData originalDirectory, newDirectory;

        newDirectory.name = dlgStr;
        newDirectory.ID = m_DirectoryMap[selectedItem].ID;
        originalDirectory.ID = m_DirectoryMap[selectedItem].ID;
        originalDirectory.name = m_DirectoryMap[selectedItem].name;


        if(TelephoneDirectoryLibrary::modifyDirectory(newDirectory, originalDirectory))
        {
            params["contactName"] = newDirectory.name;
            m_pTransactiveInterfaces->submitAuditMessage(TA_Base_Core::TelephoneManagerAudit::TeleContactModified, params);
        }
        else
        {
            TA_Base_Bus::TransActiveMessage tMsg;
            tMsg.showMsgBox(IDS_UE_620003);

            FUNCTION_EXIT;
            return;
        }
    }
    else
    {
        TelephoneDirectoryEntryData originalEntry, newEntry;
        CString tmpStr;

        pMainWnd->GetDlgItemText(IDC_SELECTED_NAME, tmpStr);
        newEntry.fullName = tmpStr;

        pMainWnd->GetDlgItemText(IDC_SELECTED_LOCATION, tmpStr);
        newEntry.location = tmpStr;

        pMainWnd->GetDlgItemText(IDC_SELECTED_NUMBER, tmpStr);
        newEntry.number = tmpStr;

        m_DirectoryEntryMap[selectedItem].directoryDataName = dlgStr;
        newEntry.ID = m_DirectoryEntryMap[selectedItem].ID;
        newEntry.directoryDataName = m_DirectoryEntryMap[selectedItem].directoryDataName;

        originalEntry.ID = m_DirectoryEntryMap[selectedItem].ID;
        originalEntry.directoryDataName = m_DirectoryEntryMap[selectedItem].directoryDataName;
        originalEntry.fullName = m_DirectoryEntryMap[selectedItem].fullName;
        originalEntry.location = m_DirectoryEntryMap[selectedItem].location;
        originalEntry.number = m_DirectoryEntryMap[selectedItem].number;

        try
        {
            if(TelephoneDirectoryLibrary::modifyEntry(newEntry, originalEntry))
            {
                params["contactName"] = dlgStr;
                m_pTransactiveInterfaces->submitAuditMessage(TA_Base_Core::TelephoneManagerAudit::TeleSubdirectoryAdded, params);
            }
            else
            {
                TA_Base_Bus::TransActiveMessage tMsg;
                tMsg.showMsgBox(IDS_UE_620003);

                FUNCTION_EXIT;
                return;
            }
        }
        catch(TelephoneDirectoryLibraryException& e)
        {
            std::ostringstream reason;
            reason << "TelephoneDirectoryLibraryException caught [" << e.what() << "]";
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, reason.str().c_str());

            FUNCTION_EXIT;
            return;
        }

    }

    FUNCTION_EXIT;
}


BOOL CTelephoneConfigureDirectoryPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPropertyPage::OnInitDialog();

    CTelephoneManagerDlg* pMainWnd = dynamic_cast<CTelephoneManagerDlg*>(AfxGetMainWnd());
    ASSERT_VALID(pMainWnd);

    m_initialThread = new CTelephoneConfigureDirectoryPageHelperInitialThread( *this, pMainWnd );
    m_initialThread->start();

    FUNCTION_EXIT;
    return TRUE;
}


void CTelephoneConfigureDirectoryPage::LoadData()
{
    FUNCTION_ENTRY( "LoadData" );

    TA_ASSERT(m_pTransactiveInterfaces != NULL, "m_pTransactiveInterfaces is NULL; Call setItaSummary first.");
    vector<TelephoneDirectoryEntryData> entries;
    vector<TelephoneDirectoryData> directories;

    try
    {
        directories = TelephoneDirectoryLibrary::getAllDirectories();
        unsigned int i = 0;

        HTREEITEM rootDirHdl = m_DirectoryTree.InsertItem(ALL.c_str());
        HTREEITEM dirHdl;
        for(i = 0; i < directories.size(); i++)
        {
            dirHdl = m_DirectoryTree.InsertItem(directories[i].name.c_str(), rootDirHdl);
            m_DirectoryMap[dirHdl] = directories[i];
        }

        entries = TelephoneDirectoryLibrary::getAllEntries();
        for(i = 0; i < entries.size(); i++)
        {
            HTREEITEM entryHdl;

            TEL_DIR_MAP::iterator iter =
                searchForDirectoryByDirName(m_DirectoryMap, entries[i].directoryDataName);

            if(iter == m_DirectoryMap.end()) // not found => new item
            {
                // add under the root
                entryHdl = m_DirectoryTree.InsertItem(entries[i].fullName.c_str(),rootDirHdl);
            }
            else
            {
                // add under the root
                entryHdl = m_DirectoryTree.InsertItem(entries[i].fullName.c_str(),(*iter).first);
            }

            m_DirectoryEntryMap[entryHdl] = entries[i];
        }
    }
    catch(TelephoneDirectoryLibraryException& e)
    {
        std::ostringstream reason;
        reason << "TelephoneDirectoryLibraryException caught [" << e.what() << "]";
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, reason.str().c_str());

        FUNCTION_EXIT;
        return;
    }

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectoryPage::OnDeleteClicked()
{
    FUNCTION_ENTRY( "OnDeleteClicked" );

    HTREEITEM selectedItem = m_DirectoryTree.GetSelectedItem();
    TA_ASSERT(selectedItem != NULL, "Nothing in tree control selected?");

    CString sItemName = m_DirectoryTree.GetItemText(selectedItem);
    string itemName = sItemName;

    if(m_DirectoryTree.ItemHasChildren(selectedItem)) // directory selected
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OnDeleteClicked(). Tried to delete a subdirectory %s that was not empty.", itemName.c_str());
        TA_Base_Bus::TransActiveMessage tMsg;
        tMsg.showMsgBox(IDS_UE_620004);

        FUNCTION_EXIT;
        return;
    }

    HTREEITEM rootItem = m_DirectoryTree.GetRootItem();

    TelephoneDirectoryData directoryData;
    TEL_DIR_MAP::iterator iterDir = m_DirectoryMap.find(selectedItem);

    if(iterDir != m_DirectoryMap.end()) // directory selected
    {
        // TD14164 ++
        /*int retVal = AfxMessageBox(_T("Are you sure you want to delete the directory?"), MB_YESNO);*/
        TA_Base_Bus::TransActiveMessage userMsg;
        CString fieldName = "directory";
        userMsg << fieldName;
        UINT retVal = userMsg.showMsgBox(IDS_UW_620001);
        // ++ TD14164
        if(IDNO == retVal)
        {
            FUNCTION_EXIT;
            return;
        }

        HTREEITEM entryHdl = NULL;
        HTREEITEM dirHdl = (*iterDir).first;

        if(m_DirectoryTree.GetParentItem(selectedItem) == rootItem)
        {
            directoryData.ID = m_DirectoryMap[selectedItem].ID;
            directoryData.name = m_DirectoryMap[selectedItem].name;

            try
            {
                if(TelephoneDirectoryLibrary::deleteDirectory(directoryData))
                {
                    TA_Base_Core::NameValuePairs params;
                    params["contactName"] = directoryData.name;
                    m_pTransactiveInterfaces->submitAuditMessage(TA_Base_Core::TelephoneManagerAudit::TeleSubdirectoryDeleted, params);
                }
                else
                {
                    TA_Base_Bus::TransActiveMessage tMsg;
                    tMsg.showMsgBox(IDS_UE_620001);
                }
            }
            catch(TelephoneDirectoryLibraryException& e)
            {
                std::ostringstream reason;
                reason << "TelephoneDirectoryLibraryException caught [" << e.what() << "]";
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, reason.str().c_str());

                FUNCTION_EXIT;
                return;
            }
        }
        else
        {
            TA_Base_Bus::TransActiveMessage tMsg;
            tMsg.showMsgBox(IDS_UE_620001);
        }
    }
    else // Entry selected
    {
        // TD14164 ++
        /*int retVal = AfxMessageBox(_T("Are you sure you want to delete the telephone entry?"), MB_YESNO);*/
        TA_Base_Bus::TransActiveMessage userMsg;
        CString fieldName = "telephone entry";
        userMsg << fieldName;
        UINT retVal = userMsg.showMsgBox(IDS_UW_620001);
        // ++ TD14164
        if(IDNO == retVal)
        {
            FUNCTION_EXIT;
            return;
        }

        TelephoneDirectoryEntryData entryData;

        entryData.directoryDataName = m_DirectoryEntryMap[selectedItem].directoryDataName;
        entryData.ID = m_DirectoryEntryMap[selectedItem].ID;
        entryData.fullName = m_DirectoryEntryMap[selectedItem].fullName;
        entryData.location = m_DirectoryEntryMap[selectedItem].location;
        entryData.number = m_DirectoryEntryMap[selectedItem].number;

        try
        {
            if(TelephoneDirectoryLibrary::deleteEntry(entryData))
            {

                TA_Base_Core::NameValuePairs params;
                params["contactName"] = entryData.fullName;
                m_pTransactiveInterfaces->submitAuditMessage(TA_Base_Core::TelephoneManagerAudit::TeleContactDeleted, params);
            }
            else
            {
                TA_Base_Bus::TransActiveMessage tMsg;
                tMsg.showMsgBox(IDS_UE_620001);
            }
        }
        catch(TelephoneDirectoryLibraryException& e)
        {
            std::ostringstream reason;
            reason << "TelephoneDirectoryLibraryException caught [" << e.what() << "]";
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, reason.str().c_str());

            FUNCTION_EXIT;
            return;
        }
    }

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectoryPage::setItaSummary(ItaTelephoneManagerSummary* pTransactiveInterfaces)
{
    FUNCTION_ENTRY( "setItaSummary" );

    m_pTransactiveInterfaces = pTransactiveInterfaces;
    TA_ASSERT(m_pTransactiveInterfaces != NULL, "m_pTransactiveInterfaces is NULL");

    FUNCTION_EXIT;
}


void CTelephoneConfigureDirectoryPage::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnSelchangedTree" );

    CTelephoneManagerDlg* pMain = dynamic_cast<CTelephoneManagerDlg*>(AfxGetMainWnd());
    ASSERT_VALID(pMain);

    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
    HTREEITEM treeItem = m_DirectoryTree.GetSelectedItem();

    CWnd *pMainWnd = AfxGetMainWnd();

    if(m_DirectoryTree.GetItemText(treeItem) == ALL.c_str())
    {
        GetDlgItem(IDC_NEW_BTN)->EnableWindow(FALSE);
        if(m_bIsPermitted)
        {
            GetDlgItem(IDC_NEW_BTN)->EnableWindow(TRUE);
        }

        GetDlgItem(IDC_DELETE_BTN)->EnableWindow(FALSE);
        GetDlgItem(IDC_UPDATE_BTN)->EnableWindow(FALSE);

        pMainWnd->GetDlgItem(IDC_SELECTED_NUMBER)->EnableWindow(FALSE);
        pMainWnd->GetDlgItem(IDC_SELECTED_LOCATION)->EnableWindow(FALSE);
        pMainWnd->GetDlgItem(IDC_SELECTED_NAME)->EnableWindow(FALSE);
        pMainWnd->GetDlgItem(IDC_SELECTED_DIRECTORY)->EnableWindow(TRUE);
    }
    else
    {
        if(m_bIsPermitted == true)
        {
            GetDlgItem(IDC_NEW_BTN)->EnableWindow(TRUE);
            GetDlgItem(IDC_DELETE_BTN)->EnableWindow(TRUE);
            GetDlgItem(IDC_UPDATE_BTN)->EnableWindow(TRUE);
        }

        if(treeItem)
        {
            if(m_DirectoryEntryMap.find(treeItem) != m_DirectoryEntryMap.end())
            {
                pMainWnd->SetDlgItemText(IDC_SELECTED_NAME,      m_DirectoryEntryMap[treeItem].fullName.c_str());
                pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER,    m_DirectoryEntryMap[treeItem].number.c_str());
                pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION,  m_DirectoryEntryMap[treeItem].location.c_str());
                pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, m_DirectoryEntryMap[treeItem].directoryDataName.c_str());

                pMainWnd->GetDlgItem(IDC_SELECTED_DIRECTORY)->EnableWindow(FALSE);
                pMainWnd->GetDlgItem(IDC_SELECTED_NAME)->EnableWindow(TRUE);
                pMainWnd->GetDlgItem(IDC_SELECTED_NUMBER)->EnableWindow(TRUE);
                pMainWnd->GetDlgItem(IDC_SELECTED_LOCATION)->EnableWindow(TRUE);
            }
            else if(m_DirectoryMap.find(treeItem) != m_DirectoryMap.end())
            {
                pMainWnd->SetDlgItemText(IDC_SELECTED_NAME,     _T(""));
                pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER,   _T(""));
                pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, _T(""));
                pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, m_DirectoryMap[treeItem].name.c_str());

                pMainWnd->GetDlgItem(IDC_SELECTED_NAME)->EnableWindow(TRUE);
                pMainWnd->GetDlgItem(IDC_SELECTED_NUMBER)->EnableWindow(TRUE);
                pMainWnd->GetDlgItem(IDC_SELECTED_LOCATION)->EnableWindow(TRUE);
                pMainWnd->GetDlgItem(IDC_SELECTED_DIRECTORY)->EnableWindow(TRUE);
            }
        }
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


BOOL CTelephoneConfigureDirectoryPage::OnSetActive()
{
    FUNCTION_ENTRY( "OnSetActive" );

    HTREEITEM treeItem = m_DirectoryTree.GetSelectedItem();
    HTREEITEM rootItem = m_DirectoryTree.GetRootItem();

    CWnd *pMainWnd = AfxGetMainWnd();
    ASSERT_VALID(pMainWnd);

    if(treeItem == rootItem)
    {
        GetDlgItem(IDC_NEW_BTN)->EnableWindow(FALSE);
        if(m_bIsPermitted)
        {
            GetDlgItem(IDC_NEW_BTN)->EnableWindow(TRUE);
        }

        GetDlgItem(IDC_DELETE_BTN)->EnableWindow(FALSE);
        GetDlgItem(IDC_UPDATE_BTN)->EnableWindow(FALSE);

        pMainWnd->GetDlgItem(IDC_SELECTED_NUMBER)->EnableWindow(FALSE);
        pMainWnd->GetDlgItem(IDC_SELECTED_LOCATION)->EnableWindow(FALSE);
        pMainWnd->GetDlgItem(IDC_SELECTED_NAME)->EnableWindow(FALSE);
        pMainWnd->GetDlgItem(IDC_SELECTED_DIRECTORY)->EnableWindow(TRUE);
    }
    else
    {
        if(m_bIsPermitted == true)
        {
            GetDlgItem(IDC_NEW_BTN)->EnableWindow(TRUE);
            GetDlgItem(IDC_DELETE_BTN)->EnableWindow(TRUE);
            GetDlgItem(IDC_UPDATE_BTN)->EnableWindow(TRUE);
        }

        if(treeItem)
        {
            if(m_DirectoryEntryMap.find(treeItem) != m_DirectoryEntryMap.end())
            {
                pMainWnd->SetDlgItemText(IDC_SELECTED_NAME,      m_DirectoryEntryMap[treeItem].fullName.c_str());
                pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER,    m_DirectoryEntryMap[treeItem].number.c_str());
                pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION,  m_DirectoryEntryMap[treeItem].location.c_str());
                pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, m_DirectoryEntryMap[treeItem].directoryDataName.c_str());

                pMainWnd->GetDlgItem(IDC_SELECTED_DIRECTORY)->EnableWindow(FALSE);
                pMainWnd->GetDlgItem(IDC_SELECTED_NAME)->EnableWindow(TRUE);
                pMainWnd->GetDlgItem(IDC_SELECTED_NUMBER)->EnableWindow(TRUE);
                pMainWnd->GetDlgItem(IDC_SELECTED_LOCATION)->EnableWindow(TRUE);
            }
            else if(m_DirectoryMap.find(treeItem) != m_DirectoryMap.end())
            {
                pMainWnd->SetDlgItemText(IDC_SELECTED_NAME,     _T(""));
                pMainWnd->SetDlgItemText(IDC_SELECTED_NUMBER,   _T(""));
                pMainWnd->SetDlgItemText(IDC_SELECTED_LOCATION, _T(""));
                pMainWnd->SetDlgItemText(IDC_SELECTED_DIRECTORY, m_DirectoryMap[treeItem].name.c_str());

                pMainWnd->GetDlgItem(IDC_SELECTED_NAME)->EnableWindow(TRUE);
                pMainWnd->GetDlgItem(IDC_SELECTED_NUMBER)->EnableWindow(TRUE);
                pMainWnd->GetDlgItem(IDC_SELECTED_LOCATION)->EnableWindow(TRUE);
                pMainWnd->GetDlgItem(IDC_SELECTED_DIRECTORY)->EnableWindow(TRUE);
            }
        }
        else
        {
            pMainWnd->GetDlgItem(IDC_SELECTED_NUMBER)->EnableWindow(FALSE);
            pMainWnd->GetDlgItem(IDC_SELECTED_LOCATION)->EnableWindow(FALSE);
            pMainWnd->GetDlgItem(IDC_SELECTED_NAME)->EnableWindow(FALSE);
            pMainWnd->GetDlgItem(IDC_SELECTED_DIRECTORY)->EnableWindow(FALSE);
        }
    }

    FUNCTION_EXIT;
    return CPropertyPage::OnSetActive();
}


