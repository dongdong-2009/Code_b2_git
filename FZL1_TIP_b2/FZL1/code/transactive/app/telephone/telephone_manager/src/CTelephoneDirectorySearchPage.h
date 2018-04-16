/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/CTelephoneDirectorySearchPage.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Class implements the search functionality for the telephone manager
  * It handles the "Search" Property Page.
  */

#if !defined(AFX_TELEPHONEDIRECTORYSEARCHPAGE_H__178A56CA_FAC9_4322_810B_19AFD203F2B1__INCLUDED_)
#define AFX_TELEPHONEDIRECTORYSEARCHPAGE_H__178A56CA_FAC9_4322_810B_19AFD203F2B1__INCLUDED_

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <map>
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryData.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryEntryData.h"

class ItaTelephoneManagerSummary;
class CTelephoneManagerDlg;
class CTelephoneDirectorySearchPageInitialThread;

class CTelephoneDirectorySearchPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CTelephoneDirectorySearchPage)
        
    friend class CTelephoneDirectorySearchPageInitialThread;

// Construction
public:
    CTelephoneDirectorySearchPage();
    ~CTelephoneDirectorySearchPage();

// Dialog Data
    // {{AFX_DATA(CTelephoneDirectorySearchPage)
    enum { IDD = IDD_FACILITY_SEARCH };
    TA_Base_Bus::AutoSortListCtrl    m_SearchList;
    // CListCtrl    m_SearchList;
    CTreeCtrl    m_DirectoryTree;
    CString    m_SearchItem;
    // }}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    // {{AFX_VIRTUAL(CTelephoneDirectorySearchPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    // }}AFX_VIRTUAL
public:

	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:

    /**
      * setItaSummary
      *
      * Assigne iSummary to m_Summary
      *
      * @return void
      * @param : ItaTelephoneManagerSummary* iSummary
      *
      * @exception <exceptions> Optional
      */
    void setItaSummary(ItaTelephoneManagerSummary* iSummary);

protected:


    /**
      * UpdateRights
      *
      * This method will allow/disallow access to features based on the operator's
      * profile. Do any enabling/disabling of GUI components in here. This method will
      * utilise the isFunctionPermitted method of the CTelephoneManagerDlg to determine
      * the appropriate rights.
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void UpdateRights();

protected:
    // Generated message map functions
    // {{AFX_MSG(CTelephoneDirectorySearchPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnSearchClicked();
    afx_msg void OnClickSearchListing(NMHDR* pNMHDR, LRESULT* pResult);
       afx_msg void OnChangeEditSearchText();
    // }}AFX_MSG

    /**
      * OnDirectoryReload
      *
      * To receive notifications of telephone directory uploades. The whole directory
      * have to be re-loaded and the m_DirectoryTree updated. Each
      * TelephoneDirectoryData item will be added to the m_DirectoryTree and then the
      * m_DirectoryMap. The item will be inserted into the m_DirectoryTree under the
      * root HTREEITEM ("All") as the parent. After each TelephoneDirectoryData entry
      * have been added, all TelephoneDirectoryEntryData items linked to the
      * TelephoneDirectoryData item will be added to the m_DirectoryTree with the
      * DirectoryEntry entry as the parent,  and will also be added to the
      * m_DirectoryEntryMap. This method is invoked by the message loop on receipt of a
      * WM_DIRECTORY_UPLOAD message.
      *
      * @return afx_msg LRESULT
      * @param : WPARAM param1 (unused)
      * @param : LPARAM param2 (unused)
      *
      * @exception <exceptions> Optional
      */
    afx_msg LRESULT OnDirectoryReload(WPARAM param1, LPARAM param2);

    /**
      * OnDirectoryEntryAdded
      *
      * This will add the TelephoneDirectoryEntryData item to the m_DirectoryTree and
      * then update the item in the m_DirectoryEntryMap. The item will be inserted into
      * the m_DirectoryTree under the parent HTREEITEM for the parent directory. The
      * parent HTREEITEM will be the key of the TelephoneDirectoryData item stored in
      * the m_DirectoryMap, who's 'name' member matches the TelephoneDirectoryEntryData
      * item's 'directoryDataName' member . This method is invoked by the message loop
      * on receipt of a WM_DIRECTORY_ENTRY_ADDED message.
      *
      * @return afx_msg LRESULT
      * @param : WPARAM param1
      *          This contains a pointer to a TelephoneDirectoryEntryData object.
      *          It should be cast to this before being used
      * @param : LPARAM param2 (unused)
      *
      * @exception <exceptions> Optional
      */
    afx_msg LRESULT OnDirectoryEntryAdded(WPARAM param1, LPARAM param2);

    /**
      * OnDirectoryEntryModified
      *
     * This will update the TelephoneDirectoryEntryData item in the m_DirectoryTree
     * matching the name of the poldDirectoryEntryData parameter with the name of the
     * pnewDirectoryEntryData and then replace the item in the m_DirectoryEntryMap
     * with the pnewDirectoryEntryData. This method is invoked by the message loop on
     * receipt of a WM_DIRECTORY_ENTRY_MODIFIED message.
      *
      * @return afx_msg LRESULT
      * @param : WPARAM param1
      *          This contains a pointer to the TelephoneDirectoryEntryData object
      *          that will be replacing the TelephoneDirectoryEntryData object being
      *          modified. It should be cast to this before being used.
      * @param : LPARAM param2
      *          This contains a pointer to the TelephoneDirectoryEntryData object
      *          that will be modified. It should be cast to this before being used.
      *
      * @exception <exceptions> Optional
      */
    afx_msg LRESULT OnDirectoryEntryModified(WPARAM param1, LPARAM param2);

    /**
      * OnDirectoryEntryDeleted
      *
      * This will delete the TelephoneDirectoryEntryData item in the m_DirectoryTree
      * and then delete the item stored in the m_DirectoryEntryMap. This method is
      * invoked by the message loop on receipt of a WM_DIRECTORY_ENTRY_DELETED message.
      *
      * @return afx_msg LRESULT
      * @param : WPARAM param1
      *          This contains a pointer to a TelephoneDirectoryEntryData object.
      *          It should be cast to this before being used.
      * @param : LPARAM param2 (unused)
      *
      * @exception <exceptions> Optional
      */
    afx_msg LRESULT OnDirectoryEntryDeleted(WPARAM param1, LPARAM param2);

    /**
      * OnDirectoryAdded
      *
      * This will add the TelephoneDirectoryData item to the m_DirectoryTree and then
      * update the item in the m_DirectoryMap. The item will be inserted into the
      * m_DirectoryTree under the root HTREEITEM ("All") as the parent. This method is
      * invoked by the message loop on receipt of a WM_DIRECTORY_ENTRY_ADDED message.
      *
      * @return afx_msg LRESULT
      * @param : WPARAM param1
      *          This contains a pointer to a TelephoneDirectoryEntryData object.
      *          It should be cast to this before being used.
      * @param : LPARAM param2 (unused)
      *
      * @exception <exceptions> Optional
      */
    afx_msg LRESULT OnDirectoryAdded(WPARAM param1, LPARAM param2);

    /**
      * OnDirectoryModified
      *
      * This will modify the TelephoneDirectoryData item in the m_DirectoryTree and
      * then update the item in the m_DirectoryMap. The item will be inserted into the
      * m_DirectoryTree under the root HTREEITEM ("All") as the parent. This method is
      * invoked by the message loop on receipt of a WM_DIRECTORY_ENTRY_ADDED message.
      *
      * @return afx_msg LRESULT
      * @param : WPARAM param1
      *          This contains a pointer to a TelephoneDirectoryEntryData object.
      *          It should be cast to this before being used. (New Data)
      * @param : LPARAM param2
      *          This contains a pointer to a TelephoneDirectoryEntryData object.
      *          It should be cast to this before being used. (Original Data)
      *
      * @exception <exceptions> Optional
      */
    afx_msg LRESULT OnDirectoryModified(WPARAM param1, LPARAM param2);

    /**
     * @param pdirectoryData    This contains a pointer to a TelephoneDirectoryData
     * object. It should be cast to this before being used.
     *
     */

    /**
      * OnDirectoryDeleted
      *
      * This will delete the TelephoneDirectoryData item from the m_DirectoryTree and
      * then delete the item from the m_DirectoryMap. This method is invoked by the
      * message loop on receipt of a WM_DIRECTORY_ENTRY_ADDED message.
      *
      * @return afx_msg LRESULT
      * @param : WPARAM param1
      *          This contains a pointer to a TelephoneDirectoryEntryData object.
      *          It should be cast to this before being used. (New Data)
      * @param : LPARAM param2 (unused)
      *
      * @exception <exceptions> Optional
      */
    afx_msg LRESULT OnDirectoryDeleted(WPARAM param1, LPARAM param2);

    DECLARE_MESSAGE_MAP()

private:

    /**
      * SortInfo for TA_Base_Bus::AutoSortListCtrl
      */
    TA_Base_Bus::AutoSortListCtrl::SortInfo m_SortInfo;

    /**
     * Used to map a TelephoneDirectoryEntryData item to the corresponding item in the
     * CTreeCtrl.
     */
    std::map<HTREEITEM, TelephoneDirectoryEntryData> m_DirectoryEntryMap;

    /**
     * Used to map a TelephoneDirectoryData item to the corresponding item in the
     * CTreeCtrl.
     */
    std::map<HTREEITEM, TelephoneDirectoryData> m_DirectoryMap;

    /**
     * Holds a reference to the CtaTelephoneManagerSummary.
     * Used by search page to retrieve session ID
     */
    ItaTelephoneManagerSummary* m_Summary;


    static const int            COLUMN_INDEX;
    static const std::string    COLUMN_NAME;
    static const int            COLUMN_OFFSET;

    CTelephoneDirectorySearchPageInitialThread* m_initialThread;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TELEPHONEDIRECTORYSEARCHPAGE_H__178A56CA_FAC9_4322_810B_19AFD203F2B1__INCLUDED_)
