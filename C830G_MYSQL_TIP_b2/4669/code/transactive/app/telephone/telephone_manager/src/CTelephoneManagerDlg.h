/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/CTelephoneManagerDlg.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  */

#if !defined(AFX_CTELEPHONEMANAGERDLG_H__DACA4218_AC2C_4A0F_A54B_2AC320AE6347__INCLUDED_)
#define AFX_CTELEPHONEMANAGERDLG_H__DACA4218_AC2C_4A0F_A54B_2AC320AE6347__INCLUDED_

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "app/telephone/telephone_manager/src/ITelephoneDirectoryListener.h"
#include "app/telephone/telephone_manager/src/ITelephoneDirectLinesListener.h"
#include "app/telephone/telephone_manager/src/CTelephoneManagerDlgStatusBar.h"
#include "app/telephone/telephone_manager/src/CTelephoneFacilitiesPropertySheet.h"
#include "app/telephone/telephone_manager/src/TelephoneRightsInitializThread.h"


class ItaTelephoneManagerSummary;
class TelephoneDirectLinesMessageSubscriber;
struct TelephoneSpeedDialButtonDetails;


class CTelephoneManagerDlg : public TA_Base_Bus::TransActiveDialog,
                             public ITelephoneDirectLinesListener,
                             public ITelephoneDirectoryListener,
                             public TelephoneRightsInitializThreadUserProcessRights
{
// Construction
public:
    CTelephoneManagerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback,
                         CWnd* pParent = NULL);

    virtual ~CTelephoneManagerDlg();

    void setTelephoneManagerSummary(ItaTelephoneManagerSummary *ptransactiveInterfaces);
    
    virtual void processTelephoneRights( const TelephoneRights& rights );

// Dialog Data
    // {{AFX_DATA(CTelephoneManagerDlg)
    enum { IDD = IDD_TELEPHONE_MANAGER_DIALOG };
    CString    m_Name;
    CString    m_Directory;
    CString    m_Number;
    CString    m_Location;
    // }}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CTelephoneManagerDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    // }}AFX_VIRTUAL

// Implementation
protected:
      HICON m_hIcon;
    ItaTelephoneManagerSummary * m_ptransactiveInterfaces;
    CTelephoneManagerDlgStatusBar m_wndStatusBar;
    CTelephoneFacilitiesPropertySheet m_telephoneFacilities;

    CToolBar m_wndToolBar;

    // Generated message map functions
    // {{AFX_MSG(CTelephoneManagerDlg)
    afx_msg void OnViewStatusbar();
    afx_msg void OnUploadDirectoryClicked();
    virtual BOOL OnInitDialog();
    afx_msg void OnExit();
    afx_msg void OnHelpAbouttelephonecontroller();
    afx_msg void OnViewToolbar();
    afx_msg void OnMove(int x, int y);
    afx_msg void onHelpTelephoneControl();
    // TD18095, jianghp, to fix the performance of showing manager application
    afx_msg void onClose();
    // TD18095, jianghp, to fix the performance of showing manager application
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()

    /**
      * SpeedDialButtonModified
      *
      * A Telephone Speed Dial button have been modified by another operator.  This
      * will send a WM_DIRECTLINES_MODIFIED message to the following windows?message
      * queues to perform the necessary updates: CTelephoneDirectLinePage
      * CTelephoneConfigureDirectLinesPage
      *
      * @return void
      * @param : const TelephoneSpeedDialButtonDetails& btnDetails
      *          The detials of the changed button.
      *
      * @exception <exceptions> Optional
      */
    void SpeedDialButtonModified(const TelephoneSpeedDialButtonDetails& btnDetails);

    /**
      * ReloadDirectory
      *
      * The telephone directory have been uploaded by another operator. This will send
      * a WM_DIRECTORY_UPLOAD message message to the following windows?message queues
      * to perform the necessary updates: CTelephoneDirectorySearchPage
      * CTelephoneConfigureDirectoryPage
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void ReloadDirectory();

    /**
      * DirectoryEntryAdded
      *
      * A TelephoneDirectoryEntryData item have been added by another operator.  This
       * will send a WM_DIRECTORY_ENTRY_ADDED message to the following windows?message
       * queues to perform the necessary updates: CTelephoneDirectorySearchPage
       * CTelephoneConfigureDirectoryPage
      *
      * @return void
      * @param : const TelephoneDirectoryEntryData& pdirectoryEntryData
      *
      * @exception <exceptions> Optional
      */
    void DirectoryEntryAdded(const TelephoneDirectoryEntryData& pdirectoryEntryData);

    /**
      * DirectoryEntryModified
      *
       * A TelephoneDirectoryEntryData item have been modified by another operator.
      * This will send a WM_DIRECTORY_ENTRY_MODIFIED message to the following windows?
      * message queues to perform the necessary updates: CTelephoneDirectorySearchPage
      * CTelephoneConfigureDirectoryPage
      *
      * @return void
      * @param : const TelephoneDirectoryEntryData& pdirectoryEntryData
      *
      * @exception <exceptions> Optional
      */
    void DirectoryEntryModified(const TelephoneDirectoryEntryData& pdirectoryEntryData);

    /**
      * DirectoryEntryDeleted
      *
      * A TelephoneDirectoryEntryData item have been deleted by another operator.  This
      * will send a WM_DIRECTORY_ENTRY_DELETED message to the following windows?
      * message queues to perform the necessary updates: CTelephoneDirectorySearchPage
       * CTelephoneConfigureDirectoryPage
      *
      * @return void
      * @param : const TelephoneDirectoryEntryData& pdirectoryEntryData
      *
      * @exception <exceptions> Optional
      */
    void DirectoryEntryDeleted(const TelephoneDirectoryEntryData& pdirectoryEntryData);

    /**
      * DirectoryAdded
      *
       * A TelephoneDirectoryData item have been added by another operator. This will
      * send a WM_DIRECTORY_ADDED message to the following windows?message queues to
       * perform the necessary updates: CTelephoneDirectorySearchPage
      * CTelephoneConfigureDirectoryPage
      *
      * @return void
      * @param : const TelephoneDirectoryData& pdirectoryData
      *
      * @exception <exceptions> Optional
      */
    void DirectoryAdded(const TelephoneDirectoryData& pdirectoryData);

    /**
      * DirectoryModified
      *
       * A TelephoneDirectoryData item have been modified by another operator. This will
      * send a WM_DIRECTORY_MODIFIED message to the following windows?message queues
      * to perform the necessary updates: CTelephoneDirectorySearchPage
      * CTelephoneConfigureDirectoryPage
      *
      * @return void
      * @param : const TelephoneDirectoryData& pdirectoryData
      *
      * @exception <exceptions> Optional
      */
    void DirectoryModified(const TelephoneDirectoryData& pdirectoryData);

    /**
      * DirectoryDeleted
      *
      * A TelephoneDirectoryData item have been deleted by another operator. This will
      * send a WM_DIRECTORY_DELETED message to the following windows?message queues to
      * perform the necessary updates: CTelephoneDirectorySearchPage
      * CTelephoneConfigureDirectoryPage
      *
      * @return void
      * @param : const TelephoneDirectoryData& pdirectoryData
      *
      * @exception <exceptions> Optional
      */
    void DirectoryDeleted(const TelephoneDirectoryData& pdirectoryData);

public:
    /**
      * isFunctionPermitted
      *
      * This method checks the rights of the operator to perform the action specified
      * by the AccessControlledAction parameter and true if the operator have the
      * privelege to carry out the specified action. This method should be called by
      * any method that needs to determine operator access to a particular feature.
      * This method will use the  isActionPermittedOnResource method of the CtaRights
      * class to determine the operator's privileges.
      *
      * @return bool
      * @param : TA_Base_Bus::AccessControlledAction action
      *          defines the action we're checking rights for
      *
      * @exception <exceptions> Optional
      */
    bool isFunctionPermitted(TA_Base_Bus::AccessControlledAction action);

    /**
      * onDegradedMode
      *
      * Allows the framework to notify the entry-point class of changes to the online
      * status of the OCC. If the OCC is down (i.e. isOccDown=true) the user should not
      * be allowed to make any configuarion changes. Thus, the user should be prevented
      * from performing any of the modification activities on the
      * CTelephoneConfigurationDirectLinesPage or the
      * CTelephoneConfigurationDirectoryPage.
      *
      * @return virtual void
      * @param : bool isOccDown
      *
      * @exception <exceptions> Optional
      */
    virtual void onDegradedMode(bool isOccDown);

    /**
      * onDirectLineModifiedEvent
      *
      * onDirectLineModifiedEvent  This must be implemented by the realised class and
      * will receive notifications of changes to the Telephone Speed Dial buttons from
      * the publisher.
      *
      * @return virtual void
      * @param : TelephoneDirectLinesMessageSubscriber* ppublisher
      *          The source of the notification
      * @param : const TelephoneSpeedDialButtonDetails& details
      *          The detials of the changed button.
      *
      * @exception <exceptions> Optional
      */
    virtual void onDirectLineModifiedEvent(TelephoneDirectLinesMessageSubscriber* ppublisher, const TelephoneSpeedDialButtonDetails& details);

    /**
      * onDirectoryEntryEvent
      *
      * onDirectoryEntryEvent  This must be implemented by the realised class and will
      * receive notifications of changes to the Telephone Directory Entries from the
      * publisher.
      *
      * @return virtual void
      * @param : TelephoneDirectoryMessageSubscriber* ppublisher
      *          The source of the notification
      * @param : ITelephoneDirectoryListener::EventType type
      *          The name of the event that occurred, e.g. "New" or "Update" or "Delete".
      * @param : const TelephoneDirectoryEntryData& directoryEntryData
      *          The data for the event.
      *
      * @exception <exceptions> Optional
      */
    virtual void onDirectoryEntryEvent(TelephoneDirectoryMessageSubscriber* ppublisher, ITelephoneDirectoryListener::EventType type, const TelephoneDirectoryEntryData& directoryEntryData);

    /**
      * onDirectoryUploadEvent
      *
      * onDirectoryUploadEvent  This must be implemented by the realised class and will
      * receive notifications of the Telephone Directory being reloaded from the
      * publisher.
      *
      * @return virtual void
      *
      * @exception <exceptions> Optional
      */
    virtual void onDirectoryUploadEvent();

    /**
      * onDirectoryEvent
      *
      * publishDirectoryEvent  This must be implemented by the realised class and will
      * receive notifications of changes to the Telephone Directory from the publisher.
      *
      * @return virtual void
      * @param : TelephoneDirectoryMessageSubscriber* ppublisher
      *          The source of the notification
      * @param : ITelephoneDirectoryListener::EventType type
      *          The name of the event that occurred, e.g. "New" or "Update" or "Delete".
      * @param : const TelephoneDirectoryData& directoryData
      *          The data for the event
      *
      * @exception <exceptions> Optional
      */
    virtual void onDirectoryEvent(TelephoneDirectoryMessageSubscriber* ppublisher, ITelephoneDirectoryListener::EventType type, const TelephoneDirectoryData& directoryData);

private:

    /**
      * loadStatusBar
      *
      * Load the status bar onto the dialog
      * Dependent on the private member m_toShowStatusBar
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void loadStatusBar();


    /**
      * loadToolBar
      *
      * Loading of toolbar onto the dialog
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void loadToolBar();

    /**
      * onTabChanged
      *
      * Used mainly for changing the status bar display
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void onTabChanged();

    /**
      * toggleToolBar
      *
      * Toggle the showing of the toolbar
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void toggleToolBar();


    /**
      * toggleStatusBar
      *
      * Toggle the showing of the status bar
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void toggleStatusBar();

private:

    // Used in the auditing messages
    static const std::string UPLOADED_MESSAGE;
    static const std::string FILE_UPLOADED_MESSAGE;

    bool m_toShowStatusBar;
    bool m_toShowToolBar;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTELEPHONEMANAGERDLG_H__DACA4218_AC2C_4A0F_A54B_2AC320AE6347__INCLUDED_)
