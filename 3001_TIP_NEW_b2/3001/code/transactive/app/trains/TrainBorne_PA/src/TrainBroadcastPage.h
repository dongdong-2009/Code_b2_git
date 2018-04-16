/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/TrainBroadcastPage.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * TrainBroadcastPage handles GUI side of train broadcast tab
  *
  */


#if !defined(AFX_TRAINBROADCASTPAGE_H__3516A4C9_EDF8_44C5_A8B8_51480CFF98EE__INCLUDED_)
#define AFX_TRAINBROADCASTPAGE_H__3516A4C9_EDF8_44C5_A8B8_51480CFF98EE__INCLUDED_

#include "app/trains/TrainBorne_PA/src/ITrainBroadcastPage.h"
#include "app/trains/TrainBorne_PA/src/resource.h"

#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/mfc_extensions/src/tab_control_with_disable/TabCtrlWithDisableEx.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"
#include "bus/trains/TrainSelectionComponent/src/TrainGroupComboBox.h"
#include "bus/trains/TrainSelectionComponent/src/TrainListCtrl.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelector.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"
#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"

#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "ITrainBorne.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <map>


// TD16865


// TD16865
class ColourCombo;
class TrainBroadcastModel;
class CachedMapChangedWindowNotificationHandler;
class RightsChangedWindowNotificationHandler;

// AZ++ TES 10924
//class TrainDvaVersionsPage;
// ++AZ

class TrainBroadcastPage :
    public TA_IRS_App::ITrainBorne,
    public ITrainBroadcastPage
{
    DECLARE_DYNCREATE(TrainBroadcastPage)

// Construction
public:

	typedef TA_Base_Bus::ColourCombo ColourCombo;

    TrainBroadcastPage( CWnd* pParent = NULL );
    ~TrainBroadcastPage();

    // AZ++ TES 10924
    //void setTrainDvaVersionsPage(TrainDvaVersionsPage* page);
    // ++AZ

    virtual bool TB_DoApply();

    virtual CWnd*       getWindow() { return this; }

    /**
     * postDialogCreate
     *
     * This function should be called once this property page has been initialized
     * and added to the tab control.  Some of the GetClientRect functions *do not* work
     * properly when put in OnInitDialog, driving the need for this function.  It should
     * be called before the page is to become visible (as view related setup is performed)
     *
     */
    void    postDialogCreate();

    /**
     * executeBroadcast (passthrough function to contained model)
     *
     * Executes the broadcast - taking the parameters from this page
     *
     */
    void    executeBroadcast();

    /**
     * displayDvaStatusDialog - Interface implementation
     *
     * @see ITrainBroadcastPage::displayDvaStatusDialog base class definition for more details
     *
     */
    virtual void displayDvaStatusDialog(const std::string& broadcastId);

    /**
     * displayLiveStatusDialog - Interface implementation
     *
     * @see ITrainBroadcastPage::displayLiveStatusDialog base class definition for more details
     *
     */
    virtual void displayLiveStatusDialog(const std::string& broadcastId);

    virtual void updateTrainLiveStaticGroupNames( const std::vector<std::string>& trainListNames );


    /**
     * trainSelectionChanged (interface implementation)
     *
     * @see ITrainSelectionListener::trainSelectionChanged for more details
     */
    //virtual void trainSelectionChanged(bool trainsAreSelected);
	
    // TD 15349
    /**
     * selectMessageNameInList
     *
     * This function is to iterate all the current dva message list in the page
     * and select the first message that matches the given name. Return the status of selection.
     *
     * @param std::string: Message name to be selected from the current list.
     *
     * @return bool: Status of selection. True if found and selected, otherwise False
     *
     */
    bool selectMessageNameInList(const std::string messageName);
    // TD 15349

	void refreshISCSVersion();

	virtual TA_IRS_Bus::TrainSelector& getTrainSelector();

	void TB_SetParentSelector(TA_IRS_Bus::TrainSelector* selector);

	void initialiseDVAPriorityComboData();
// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(TrainBroadcastPage)
public:
    virtual BOOL OnSetActive();
    virtual BOOL OnKillActive();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL


protected:

    /**
     * setTimeStartDtp
     *
     * Sets all the cyclical date start controls
     * so they correspond collectively to the input parameter time
     *
     * @param time the new time to update the control values with
     *
     */
    virtual void setTimeStartDtp(const CTime& time);

    /**
     * setTimeStopDtp
     *
     * Sets all the cyclical date stop controls
     * so they correspond collectively to the input parameter time
     *
     * @param time the new time to update the control values with
     *
     */
    virtual void setTimeStopDtp(const CTime& time);


// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(TrainBroadcastPage)
    afx_msg void OnTrnCyclicCb();
    afx_msg void OnTrnDvaBtn();
    virtual BOOL OnInitDialog();
    afx_msg void OnTrnLiveBtn();
    afx_msg LRESULT OnInboundTrainListUpdate(UINT wParam, LONG lParam);
    afx_msg LRESULT OnOutboundTrainListUpdate(UINT wParam, LONG lParam);
    afx_msg LRESULT OnDynamicGroupListUpdate(UINT wParam, LONG lParam);
    afx_msg LRESULT OnTrainDvaMessageUpdate(UINT wParam, LONG lParam);
    afx_msg LRESULT OnErrorMsg(WPARAM wParam, LPARAM lParam);
    afx_msg void OnDestroy();
    afx_msg void OnButtonDeleteGroup();
    afx_msg void OnButtonSaveGroupSel();
    //afx_msg void OnButtonalltrains();
    //afx_msg void OnButtonclearall();
    //afx_msg void OnSelchangingTrainlistTab(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnChangeTrnMessageFilterTb();
    //afx_msg void OnClickStnInboundSelectionLst(NMHDR* pNMHDR, LRESULT* pResult);
    //afx_msg void OnClickStnOutboundSelectionLst(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDurationChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPeriodChange(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemchangedTrnDvaMessagesLst(NMHDR* pNMHDR, LRESULT* pResult);
    //afx_msg void OnButtonalloutbound();
    //afx_msg void OnButtonallinbound();
    afx_msg LRESULT OnRightsChanged(UINT wParam, LONG lParam);
	afx_msg void OnSelchangeComboDvaPriority();
	afx_msg void OnSelchangeStatusCb();
	afx_msg void OnRestoreStatusBtn();
	afx_msg void OnKillStatusBtn();
	afx_msg void OnKillAllStatusBtn();
	afx_msg LRESULT OnStatusNameChange(UINT wParam, LONG lParam);   
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

protected:

    /**
     * processStartStopTimeChange
     *
     * Called each time there is a change in the start and stop time controls
     *
     */
    void    processStartStopTimeChange();

private:

    /**
     * refreshTrainSelectionComponentRights updates rights contained in train
     *      selection component GUI (call on initialisation, and whenever rights change)
     */
    void refreshTrainSelectionComponentRights();

    /**
     * getOutboundTrainList (simple accessor)
     */
    //TA_IRS_Bus::TrainListCtrl& getOutboundTrainList();

    /**
     * getInboundTrainList (simple accessor)
     */
    //TA_IRS_Bus::TrainListCtrl& getInboundTrainList();

    /**
     * initialiseTrainListTabSelector
     *
     * Called once off on dialog initialisation to set up the
     *  tab control that contains the train lists
     *
     */
	
    void    initialiseTrainListTabSelector();
	
    /**
     * EnableCyclicArea
     *
     * Called whenever there is a state change that may
     *  affect the enabled state of the cyclic area
     *
     * @param isEnabled true if the area should be enabled
     *
     */
    void EnableCyclicArea(bool isEnabled);

    /**
     * EnableDvaArea
     *
     * Called whenever there is a state change that may
     *  affect the enabled state of the DVA broadcast area
     *
     * @param isEnabled true if the area should be enabled
     *
     */
    void EnableDvaArea(bool isEnabled);

    /**
     * refreshTrainListState
     *
     * Called on startup and whenever there is a change in something that
     *  affects the state of the train list (not selection wise, but when
     *  privileges change etc, requiring change in enabled state)
     *
     * @return true if the refresh went successfully (acquired current
     *          mode and switched to it successfully)
     *
     */
    //bool refreshTrainListState();

    /**
     * refreshBroadcastButtonEnabledState
     *
     * Called on startup, and whenever the rights associated with these buttons changes
     *  (to update their enabled state)
     *
     */
    void refreshBroadcastButtonEnabledState();

    /**
     * enterDefaultBroadcastMode
     *
     * Called whenever we want to enter any valid broadcast state (dva, live)
     *      this is called on startup, and whenever enabled state of buttons change that
     *      cause the current selection to become invalid
     *
     */
    void enterDefaultBroadcastMode();

    /**
     * setSelectedBroadcastButton
     *
     * Updates the current broadcast mode to the new state defined by input button
     *      if the input button is null then all broadcast buttons cleared
     *
     * @param selection pointer to the button defining the new state
     *          we're supposed to enter, or NULL to clear any selection
     *
     */
    void setSelectedBroadcastButton(CButton* selection);

    /**
     * OnMessageSelectionChanged
     *
     * Called whenever the DVA message selection changes
     *
     */
    void OnMessageSelectionChanged();

    /**
     * getSelectedDvaMessageKey
     *
     * @return the primary key of the currently selected DVA Message
     *          or getSelectedDvaMessageKey() if no current selection
     *
     */
    int getSelectedDvaMessageKey();

    /**
     * getSelectedDvaMessageText
     *
     * @return the text of the currently selected DVA Message
     *          or "" if no current selection
     *
     */
	CString getSelectedDvaMessageText();

	bool getSelectedDvaMessagePriority();

    /**
     * updateDvaMessageList
     *
     * Updates our message list control with the selected messages
     *
     * @param messages the list of message records
     *
     */
    void    updateDvaMessageList(const TA_IRS_Bus::TrainDvaMessageRecords& messages);


    /**
     * getAppropriateInServiceMode
     *
     * There are two in service display modes, which vary depending on if
     * the terminal is an OCC or station terminal - this function returns
     *
     * @return the mode which should be used for this terminal for the "In Service" Tab
     *
     */
    TA_IRS_Bus::LocationCache::EStationType getAppropriateInServiceMode() const;


    /**
     * getTimeStartDtp
     *
     * @return the start time corresponding to the collection of cyclical start time
     *          date / time controls
     *
     */
    CTime getTimeStartDtp() const;

    /**
     * getTimeStopDtp
     *
     * @return the stop time corresponding to the collection of cyclical stop time
     *          date / time controls
     *
     */
    CTime getTimeStopDtp() const;

    /**
     * initialiseButtonIcons
     *
     * initializes (loads) and attaches the icon bitmaps associated
     * with the buttons on this dialog
     *
     */
    void    initialiseButtonIcons();


    /**
     * onSwitchToDepotTab
     *
     * Called when operator switches modes to InService/Depot etc
     *
     * @param mode the new display mode
     *
     * @return true if the mode switch was successful
     *
     */
    bool    switchToTrainDisplayMode(TA_IRS_Bus::LocationCache::EStationType mode);

    /**
     * formatTrainDvaMessageSelectionList
     *
     * Sets up the default formatting for the train DVA message selection list
     *
     * @param listToUpdate the list to apply the formatting to
     *
     */
    void    formatTrainDvaMessageSelectionList(CListCtrl& listToUpdate);

    /**
     * setupCyclicalBroadcastArea
     *
     * sets up the initial formatting of cyclical broadcast area
     *
     */
    void    setupCyclicalBroadcastArea();

    /**
     * updateExecuteButtonEnabledState
     *
     * Updates the PA Managers execute button state (as appropriate)
     *
     */
    void    updateExecuteButtonEnabledState();

    /**
     * getSelectedTrains
     *
     * @return an array of all the selected trains, in both inbound and outbound lists
     *
     */
    // TD16865
    std::vector<unsigned int>   getSelectedTrains();
    // TD16865

    /**
     * getTrainSelectorGUI
     *
     * @pre train selector GUI has been allocated on heap
     */
    virtual TA_IRS_Bus::TrainSelectorGUI& getTrainSelectorGUI();

	

    enum ETrainDvaMessageColumns
    {
        TDM_COL_LABEL,
        TDM_COL_MAX
    };

    // TD 15349
    /**
     * findAndSelectMessageNameInList
     *
     * This function will:- Iterate through the message list and find a message with same name as
     * submitted nameIf found:- Deselect the current selected item- Select the item with same name
     *
     * @param std::string: The message name for selection
     *
     * @return bool: True if successfully found and selected, otherwise False
     *
     */
    bool findAndSelectMessageNameInList(const std::string messageName);

    /**
     * clearMessageSelectionInList
     *
     * This is to un-select any selections in the DVA message list.
     *
     * @return bool: Return True if success, otherwise False
     *
     */
    bool clearMessageSelectionInList();


	/**
     * updateComboBoxOnNameChange
     *
     * Updates the combo box after a name change notification received
     *
     * @param id the identifier of the name change that took place
     *
     * @return true if the name change was propogated successfully to the combo box
     *
     */
    bool    updateComboBoxOnNameChange(UINT id);
    // TD 15349

	bool TB_CheckEntity(TA_Base_Core::IEntityData* guiEntity);

	void TB_PageActived(bool bActived);

	void TB_TrainSelected( TA_IRS_Bus::CommonTypes::TrainIdList selList);

	void TB_SetObserver(TA_IRS_Bus::ITrainSelectorGUI* gui=NULL);

	
    void    refreshStatusNameCombo(bool bInitialization);

    typedef TA_Base_Bus::CTabCtrlWithDisableEx CTabCtrlWithDisableEx;
// Dialog Data
    //{{AFX_DATA(TrainBroadcastPage)
    enum { IDD = IDD_TRAIN_BROADCAST };
    //CButton    m_allOutboundButton;
    //CButton    m_allInboundButton;
    //CButton    m_clearTrainSelectionButton;
    //CButton    m_selectAllTrainsButton;
    TA_IRS_Bus::TrainGroupComboBox    m_dynamicGroupCombo;
    TA_IRS_Bus::TrainStaticGroupComboBox    m_staticGroupCombo;
    CButton    m_saveDynamicGroupButton;
    CButton    m_deleteDynamicGroupButton;
    CDateTimeCtrl    m_period;
    CDateTimeCtrl    m_duration;
    TA_Base_Bus::AutoSortListCtrl    m_dvaMessageList;
    //TA_IRS_Bus::TrainListCtrl*    m_outboundTrainList;
    //TA_IRS_Bus::TrainListCtrl*    m_inboundTrainList;
    //CTabCtrlWithDisableEx    m_locationTab;
    CEdit    m_messageFilterTb;
    CButton    m_liveBtn;
    CButton    m_dvaBtn;
    CButton    m_cyclicCb;
    //CStatic    m_inboundLabel;
    //CStatic    m_outboundLabel;
	ColourCombo m_DVAPriorityCombo;
	CComboBox	m_statusNameList;
	CButton	m_restoreStatusButton;
	CButton	m_killStatusButton;
	CButton	m_killAllStatusButton;
    //}}AFX_DATA

    struct GroupCreation
    {
        std::string currentlySelectedGroup;
        std::string newGroup;
        std::vector<unsigned int> selection;
        time_t timeStamp;
    };

    // Tracks state of whether any trains are currently selected (>=1)
    bool    m_anyTrainsSelected;

    /// True while this property page is active
    bool    m_pageActive;

    TrainBroadcastModel*    m_trainBroadcastModel;
    //RightsChangedWindowNotificationHandler* m_rightsChangeHandler;

    CachedMapChangedWindowNotificationHandler* m_trainDvaUpdateNotificationHandler;

    TA_Base_Bus::FixedHeaderCtrl     m_dvaFixedHeaderCtrl;

    HANDLE  m_hLiveIcon;
    HANDLE  m_hDvaIcon;

    static const std::string s_inServiceTabText;
    static const std::string s_depotTabText;

    static const std::string    s_enterNonZeroLengthNameMessage;
    static const std::string    s_dynamicGroupNameAlreadyExistsMessage;
    static const std::string    s_defaultDynamicGroupName;
    static const std::string    s_enterDynamicGroupNamePrompt;
    static const std::string    s_enterDynamicGroupNameWindowTitle;

    // whether any trains are selected
    bool m_trainsAreSelected;

    // train selection component
    TA_IRS_Bus::TrainSelectorGUI* m_trainSelectorGUI;

    // AZ++ TES 10924
    //TrainDvaVersionsPage* m_trainDvaVersionsPage;
    // ++AZ

    // TD16865
    std::map<unsigned int, std::string> m_trainIdToServiceNumber;
    // TD16865
	TA_Base_Core::PrimitiveWrapper<TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType> m_currentStatusNameBroadcastType;

	bool m_isAtLeastTwoTrainsSelected;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAINBROADCASTPAGE_H__3516A4C9_EDF8_44C5_A8B8_51480CFF98EE__INCLUDED_)
