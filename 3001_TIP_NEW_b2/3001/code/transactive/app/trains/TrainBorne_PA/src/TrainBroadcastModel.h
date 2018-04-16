/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/TrainBroadcastModel.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * TrainBroadcastModel models behaviour for the train broadcast GUI tab of the PA Manager GUI
  */


#if !defined(AFX_TrainBroadcastModel_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define AFX_TrainBroadcastModel_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

//#include "app/trains/TrainBorne_PA/src/CachedConfig.h"
#include "app/trains/TrainBorne_PA/src/GenericBroadcastModel.h"
//#include "app/trains/TrainBorne_PA/src/PAProgressReportSubscriber.h"
#include "app/trains/TrainBorne_PA/src/TrainListUpdateContainer.h"
#include "app/trains/TrainBorne_PA/src/PaStatusNameChangeSubscriber.h"

#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/trains/TrainSelectionComponent/src/ITrainSelectorGUI.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelector.h"

#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include <string>


class ITrainBroadcastPage;


class TrainBroadcastModel 
: 
public GenericBroadcastModel,
public TA_IRS_Bus::ITrainSelectorGUI
{
public:

	enum ERightsCheckingMode
	{
		USE_RIGHTS_CHECKING,
		IGNORE_RIGHTS_CHECKING
	};
    /**
     * TrainBroadcastModel (constructor)
     *
     * @param pageView the page (view) we're associated with
     *
     * @param initialStationMode initial mode which affects what type of stations
     *          are expected to be displayed in attached view
     *
     */
      TrainBroadcastModel(ITrainBroadcastPage& pageView);

    /**
     * ~TrainBroadcastModel (destructor)
     *
     */
    virtual ~TrainBroadcastModel();


    /**
     * collectStatusNameChange
     *
     * Called when a Status Name change notification has been received by associated view
     *
     * @param id the identifier of the Status Name change packet
     *
     * @param hWnd the handle of the window that received the changed notification
     *
     * @param from [OUT] the name of the item being changed
     *
     * @param to [OUT] the name that the items being changed to
     *
     * @return the type of broadcast for this message
     *
     * @exception ValueNotSetException if the specified name change object doesn't exist
     *
     * @exception TransactiveException if we're not connected to the subscriber anymore
     *
     */
    TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType collectStatusNameChange(DWORD id, HWND hWnd, std::string& from, std::string& to);

    /**
     * updateInboundList (implemented from ITrainSelectorGUI)
     *
     * Updates the list of inbound trains for this GUI
     *
     * @param action        the action to take (see EAction definition).
     * @param trainDetails  the new details of the train if this is an Add
     *                      or Update.
     *
     */
    virtual void updateInboundList(
         TA_IRS_Bus::ITrainSelectorGUI::EAction action,
         const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails);

    /**
     * updateOutboundList (implemented from ITrainSelectorGUI)
     *
     * Updates the list of outbound trains for this GUI
     *
     * @param action        the action to take (see EAction definition).
     * @param trainDetails  the new details of the train if this is an Add
     *                      or Update.
     *
     */
    virtual void updateOutboundList(
         TA_IRS_Bus::ITrainSelectorGUI::EAction action,
         const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails);

    /**
     * updateSelectionCombo (implemented from ITrainSelectorGUI)
     *
     * Called when a selection group is added or deleted. The GUI should
     * update its selection combo with the list of train selection list
     * names.
     *
     * @param trainListNames  List of names of train selection lists that
     *                        are available.
     *
     */
    virtual void updateSelectionCombo(const std::vector<std::string>& trainListNames);

    /**
     * getPageHwnd
     *
     * @pre the associated page has a valid handle (isn't destroyed / non existent)
     *
     * @return the hWnd of the page this model is attached to
     *
     */
    HWND   getPageHwnd() const;

    /**
     * enableUpdateListener
     *
     * Has this model begin listening out for updates to the train states / groups
     *     The listener must be shut down before the associated page window is destroyed
     *
     * @pre the page attached to this model currently has a valid window (internal check)
     *
     * @param bEnable true to begin listening, false to shutdown the listener
     *
     */
    void   enableUpdateListener(bool bEnable);

    /**
     * fetchUpdatedInboundTrainData
     *
     * Should be called by associated page when it receives update waiting notifications
     *
     * This is a passthrough function, see TrainListUpdateContainer::fetchUpdate
     * for possible exceptions
     *
     */
    TrainListUpdateContainer::BufferedItem fetchUpdatedInboundTrainData();

    /**
     * fetchUpdatedOutboundTrainData
     *
     * Should be called by associated page when it receives update waiting notifications
     *
     * This is a passthrough function, see TrainListUpdateContainer::fetchUpdate
     * for possible exceptions
     *
     */
    TrainListUpdateContainer::BufferedItem fetchUpdatedOutboundTrainData();

    /**
     * fetchDynamicGroupList
     *
     * @return the list of items that should be present in the dynamic groups combo
     *
     */
    std::vector<std::string>   fetchDynamicGroupList();

    /**
     * executeBroadcast
     *
     * Executes the broadcast - taking the parameters from this model
     *
     * @param trainCoverage list of train IDs this broadcast is being sent to
     *
     */
    void    executeBroadcast(const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trainCoverage);

    /**
     * setDuration
     *
     * Sets the starting time (as date)
     *
     * @param start time in seconds from 1970 epoch
     *
     */
    virtual void setDuration(time_t start);

    /**
     * setSelectedMessage
     *
     * @param id the primary key of the train DVA message, -1 to unset
     *
     */
    virtual void setSelectedMessage(int id);

    /**
     * setPeriod
     *
     * Sets the stopping time (as date)
     *
     * @param start time in seconds from 1970 epoch
     *
     */
    virtual void setPeriod(time_t start);

    /**
     * setCyclicalEnabled
     *
     */
    void setCyclicalEnabled(bool enabled);

    /**
     * attemptTrainSubscriberConnection
     *
     * Attempts to connect the train subscriber (instance it)
     *
     * @pre the subscriber isn't already connected, that is, !isTrainSubscriberConnected()
     *
     */
    void    attemptTrainSubscriberConnection();

    /**
     * isTrainSubscriberConnected
     *
     * @return true if the train subscriber object is connected
     *
     */
    bool    isTrainSubscriberConnected() const;

    /**
     * getMessages
     *
     * @param filterKeyword the string to use to filter the result
     *
     * @return all the train DVA messages (to put in message selection list) -
     *      after filtering using keyword
     *
     */
    TA_IRS_Bus::TrainDvaMessageRecords getMessages(const std::string& filterKeyword);

    /**
     * unsetBroadcastType
     *
     * Unsets the broadcast set for this model (so no longer valid)
     *
     */
    void unsetBroadcastType();

    /**
     * setBroadcastType
     *
     * Sets the type of broadcast to be sent to PA Agent
     *
     * @param broadcastType new type of broadcast
     *
     */
    void setBroadcastType(TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType broadcastType);

    /**
     * testCanObtainLock
     *
     * Obtains the m_lock, and returns true
     *
     * @pre this thread doesn't already have the lock
     *
     */
    bool testCanObtainLock();

    /**
     * getTrainSelector (simple accessor)
     */
    TA_IRS_Bus::TrainSelector* getTrainSelector() { return m_trainSelectionSubscriber; }

    /**
     * setHasOverrideOption (simple set function)
     */
    void    setHasOverrideOption(bool hasOption) { m_hasOverrideOption = hasOption; }

    /**
     * getHasOverrideOption
     *
     * @see LiveStatusModel::getHasOverrideOption base class for details
     *
     */
    virtual bool getHasOverrideOption();

	/** 
     * confirmStatusNameExists
     *
     * Confirms the input status name (broadcast ID) exists within the PA Agent
     * If the broadcast doesn't exist, an error dialog will be displayed
     *  (this function is only to double-check/confirm a status name exists)
     * 
     * @param broadcastId the name of the status name to check
     *
     * @return true if the specified broadcast ID was found
     *
     */
    bool confirmStatusNameExists(const std::string& broadcastId);

	 /**
      * terminateAndRemoveBroadcastIds
      *
      * Passes through to static implementation, using local PA Agent as argument
      *
      */
    int terminateAndRemoveBroadcastIds(const std::vector<std::string>& broadcastIds);


	/**
     * terminateAndRemoveBroadcastIds
     *
     * Terminates and removes all the broadcastIds specified in array
     *
     * @param broadcastIds array of broadcast Ids we want to have removed
     *
     * @return number of broadcasts that couldn't be removed, because they couldn't
     *          be terminated properly (ie another terminal activating them)
     *
     * @exception ObjectResolutionNotSet if there is a CORBA resolving conflict (comm error)
     *
     * @param origin defines the creation category (origin) of this broadcast - if available
     *
     */
    static int terminateAndRemoveBroadcastIds(const std::vector<std::string>& broadcastIds,
                                                PAAgentNamedObject& paAgent,
                                                EBroadcastOrigin origin = BO_UNKNOWN);

    void    initializeStatusNameSubscriber(HWND hWnd);
    std::vector<std::string>    getTrainDvaBroadcastZones();
    static std::vector<std::string> getDvaBroadcastZones(PAAgentNamedObject& paAgent,
                                                         const TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType& filter,
                                                         ERightsCheckingMode filterByRightToView = USE_RIGHTS_CHECKING);
    void setSelectedStaticGroup(const std::string& groupName);

	void setSelectedDvaMessageText(const CString& msgText);
    virtual void updateFallbackStatus(bool inFallback);
    virtual bool hasRadioGroupSupport();
    virtual void updateRadioGroupCombo(const std::vector<std::string>& trainListNames);
    virtual TA_IRS_Bus::TrainInformationTypes::TrainDetailsList getSelectedTrainData() const;

	void TB_SetParentSelector(TA_IRS_Bus::TrainSelector* selector){	m_trainSelectionSubscriber = selector;}

	void setSelectedDvaMessagePriority(bool bEmergency){ m_bEmergency = bEmergency;};

    unsigned long getTrainDvalibraryVersion()throw (TA_Base_Core::InvalidPasConfigurationException);

    void restoreBroadcast(const std::string& broadcastId);

protected:


private:

    /**
     * executeDva
     *
     * executes a DVA play request using the current parameters
     *
     * @exception BadParameterException if there is something wrong with the parameters
     *              and the operation failed
     *
     * @exception AccessDeniedException (from PA Agent - insufficient rights)
     *
     * @exception InvalidPasConfigurationException if the library version couldn't be identified
     *
     */
    void    executeDva();

    /**
     * executeLive
     *
     * executes a live play request using the current parameters
     *
     * @exception BadParameterException if there is something wrong with the parameters
     *              and the operation failed
     *
     * @exception AccessDeniedException (from PA Agent - insufficient rights)
     *
     */
    void executeLive();

    /**
     * getSessionId
     *
     * @return the session ID of the parent GUI
     *
     */
    const std::string getSessionId() const;


    // Maintains the current station mode
    TA_IRS_Bus::LocationCache::EStationType m_currentStationMode;

    TrainListUpdateContainer m_inboundUpdateBuffer;
    TrainListUpdateContainer m_outboundUpdateBuffer;

    TA_Base_Core::NonReEntrantThreadLockable m_dynamicGroupListLock;
    std::vector<std::string> m_dynamicGroupList;

    // Page associated with this model (to receive data update notifications)
    ITrainBroadcastPage&    m_trainBroadcastPage;

    TA_IRS_Bus::TrainSelector*  m_trainSelectionSubscriber;

    time_t                                      m_period;
    time_t                                      m_duration;
    bool                                        m_cyclicalEnabled;

    TA_Base_Core::NonReEntrantThreadLockable         m_lock;

    TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage     m_trainCoverage;

    TA_Base_Core::PrimitiveWrapper<int>  m_selectedMessage;
    TA_Base_Core::PrimitiveWrapper<TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType>    m_broadcastType;

    // True if model is currnetly in override mode
    bool    m_hasOverrideOption;

    std::string m_selectedStaticGroup;

	bool m_bEmergency;

	CString m_selectedDvaMessageText;
};

#endif // !defined(AFX_TrainBroadcastModel_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
