/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_manager/src/CTelephoneConfigureDirectoryPage.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#if !defined(AFX_CTELEPHONECONFIGUREDIRECTORYPAGE_H__66B6B2F2_8E06_4AFC_9197_9B2E88BA5E9E__INCLUDED_)
#define AFX_CTELEPHONECONFIGUREDIRECTORYPAGE_H__66B6B2F2_8E06_4AFC_9197_9B2E88BA5E9E__INCLUDED_

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <map>
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryData.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryEntryData.h"

class ItaTelephoneManagerSummary;
class CTelephoneManagerDlg;
class CTelephoneConfigureDirectoryPageHelperInitialThread;

class CTelephoneConfigureDirectoryPage : public CPropertyPage
{
    friend class CTelephoneConfigureDirectoryPageHelperInitialThread;
    DECLARE_DYNCREATE(CTelephoneConfigureDirectoryPage)

// Construction
public:
    CTelephoneConfigureDirectoryPage();
    ~CTelephoneConfigureDirectoryPage();

// Dialog Data
    // {{AFX_DATA(CTelephoneConfigureDirectoryPage)
    enum { IDD = IDD_FACILITY_CONFIG_DIRECTORY };
    CTreeCtrl    m_DirectoryTree;
    // }}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    // {{AFX_VIRTUAL(CTelephoneConfigureDirectoryPage)
    public:
    virtual BOOL OnSetActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

public:
    /**
      * setItaSummary
      *
      * Assigne pTransactiveInterfaces to m_pTransactiveInterfaces
      *
      * @return void
      * @param : ItaTelephoneManagerSummary* iSummary
      *
      * @exception <exceptions> Optional
      */
    void setItaSummary(ItaTelephoneManagerSummary* pTransactiveInterfaces);

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
    void UpdateRights( CTelephoneManagerDlg* pMainWnd );

protected:
    // Generated message map functions
    // {{AFX_MSG(CTelephoneConfigureDirectoryPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnNewClicked();
    afx_msg void OnUpdateClicked();
    afx_msg void OnDeleteClicked();
    afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
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
      * TelephoneDirectoryEntry entry as the parent,  and will also be added to the
      * m_DirectoryEntryMap. This method is invoked by the message loop on receipt of a
      * WM_DIRECTORY_UPLOAD message.
      *
      * @return virtual afx_msg LRESULT
      * @param : WPARAM unused1
      * @param : LPARAM unused2
      *
      * @exception <exceptions> Optional
      */
    virtual afx_msg LRESULT OnDirectoryReload(WPARAM unused1, LPARAM unused2);

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
      * @return virtual afx_msg LRESULT
      * @param : WPARAM pdirectoryEntryData
      *          This contains a pointer to a TelephoneDirectoryEntryData object.
      *          It should be cast to this before being used.
      * @param : LPARAM unused
      *
      * @exception <exceptions> Optional
      */
    virtual afx_msg LRESULT OnDirectoryEntryAdded(WPARAM pdirectoryEntryData, LPARAM unused);

    /**
      * OnDirectoryEntryModified
      *
      * This will update the TelephoneDirectoryEntryData item in the m_DirectoryTree
      * matching the name of the poldDirectoryEntryData parameter with the name of the
      * pnewDirectoryEntryData and then replace the item in the m_DirectoryEntryMap
      * with the pnewDirectoryEntryData. This method is invoked by the message loop on
      * receipt of a WM_DIRECTORY_ENTRY_MODIFIED message.
      *
      * @return virtual afx_msg LRESULT
      * @param : WPARAM pnewDirectoryEntryData
      *          This contains a pointer to the TelephoneDirectoryEntryData object
      *          that will be replacing the TelephoneDirectoryEntryData object being modified.
      *          It should be cast to this before being used.
      * @param : LPARAM poldDirectoryEntryData
      *          This contains a pointer to the TelephoneDirectoryEntryData object that
      *          will be modified. It should be cast to this before being used.
      *
      * @exception <exceptions> Optional
      */
    virtual afx_msg LRESULT OnDirectoryEntryModified(WPARAM pnewDirectoryEntryData, LPARAM poldDirectoryEntryData);

    /**
      * OnDirectoryEntryDeleted
      *
      * This will delete the TelephoneDirectoryEntryData item in the m_DirectoryTree
      * and then delete the item stored in the m_DirectoryEntryMap. This method is
      * invoked by the message loop on receipt of a WM_DIRECTORY_ENTRY_DELETED message.
      *
      * @return virtual afx_msg LRESULT
      * @param : WPARAM pdirectoryEntryData
      *          This contains a pointer to a TelephoneDirectoryEntryData object.
      *          It should be cast to this before being used.
      * @param : LPARAM unused
      *
      * @exception <exceptions> Optional
      */
    virtual afx_msg LRESULT OnDirectoryEntryDeleted(WPARAM pdirectoryEntryData, LPARAM unused);

    /**
      * OnDirectoryAdded
      *
      * This will add the TelephoneDirectoryData item to the m_DirectoryTree and then
      * update the item in the m_DirectoryMap. The item will be inserted into the
      * m_DirectoryTree under the root HTREEITEM ("All") as the parent. This method is
      * invoked by the message loop on receipt of a WM_DIRECTORY_ENTRY_ADDED message.
      *
      * @return virtual afx_msg LRESULT
      * @param : WPARAM pdirectoryData
      *          This contains a pointer to a TelephoneDirectoryData object.
      *          It should be cast to this before being used.
      * @param : LPARAM unused
      *
      * @exception <exceptions> Optional
      */
    virtual afx_msg LRESULT OnDirectoryAdded(WPARAM pdirectoryData, LPARAM unused);

    /**
      * OnDirectoryModified
      *
      * This will modify the TelephoneDirectoryData item in the m_DirectoryTree and
      * then update the item in the m_DirectoryMap. The item will be inserted into the
      * m_DirectoryTree under the root HTREEITEM ("All") as the parent. This method is
      * invoked by the message loop on receipt of a WM_DIRECTORY_ENTRY_ADDED message.
      *
      * @return virtual afx_msg LRESULT
      * @param : WPARAM pnewDirectoryData
      *          This contains a pointer to a TelephoneDirectoryData object.
      *          It should be cast to this before being used.
      * @param : LPARAM poldDirectoryData
      *          This contains a pointer to a TelephoneDirectoryData object.
      *          It should be cast to this before being used.
      *
      * @exception <exceptions> Optional
      */
    virtual afx_msg LRESULT OnDirectoryModified(WPARAM pnewDirectoryData, LPARAM poldDirectoryData);

    /**
      * OnDirectoryDeleted
      *
      * This will delete the TelephoneDirectoryData item from the m_DirectoryTree and
      * then delete the item from the m_DirectoryMap. This method is invoked by the
      * message loop on receipt of a WM_DIRECTORY_ENTRY_ADDED message.
      *
      * @return virtual afx_msg LRESULT
      * @param : WPARAM pdirectoryData
      *          This contains a pointer to a TelephoneDirectoryData
      *          object. It should be cast to this before being used.
      * @param : LPARAM unused
      *
      * @exception <exceptions> Optional
      */
    virtual afx_msg LRESULT OnDirectoryDeleted(WPARAM pdirectoryData, LPARAM unused);

    DECLARE_MESSAGE_MAP()

private:
    /**
      * LoadData
      *
      * Helper function to load directory information from the database
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void LoadData();

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
     * A pointer to the ItaTelephoneManagerSummary
     */
    ItaTelephoneManagerSummary* m_pTransactiveInterfaces;


    bool m_bIsPermitted;

    CTelephoneConfigureDirectoryPageHelperInitialThread* m_initialThread;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTELEPHONECONFIGUREDIRECTORYPAGE_H__66B6B2F2_8E06_4AFC_9197_9B2E88BA5E9E__INCLUDED_)
