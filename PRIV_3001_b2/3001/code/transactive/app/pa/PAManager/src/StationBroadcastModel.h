/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/StationBroadcastModel.h $
  * @author Sean Liew
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  */

#if !defined(AFX_StationBroadcastModel_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define AFX_StationBroadcastModel_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/pa/PAManager/src/BroadcastParametersDlg.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/GenericBroadcastModel.h"
#include "app/pa/PAManager/src/IWindow.h"
#include "app/pa/PAManager/src/PAProgressReportSubscriber.h"

#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"

#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include <string>


class IStationBroadcastPage;

class StationBroadcastModel : public GenericBroadcastModel
{
public:
  	StationBroadcastModel(IStationBroadcastPage& page);
    virtual ~StationBroadcastModel();

    /**
     * @param selectedZones the set of zones this broadcast is to operate on
     */
    void executeBroadcast();

    void onChangeNormalFilterTb(const std::string& filterKeyword);
    void onChangeEmergencyFilterTb(const std::string& filterKeyword);
    void onAdhocListUpdate();
    void onSelectedMessagesListUpdate();
    void onClearOneBtn(int nItem);
    void onClearAllBtn();
    void addToSelectedMessagesList(unsigned long key);
    void updateZoneList( const std::vector<unsigned long>& vecZoneKeys );
    void updateCurZoneGroup( const std::string& strGroupName );
    
    /**
     * postAdhocLabelChange
     *
     * posts an adhoc label change notification to (via) the PA agent
     *
     * @param messageKey key identifying the message requiring update
     *
     * @param label the new label for the message
     *
     * @return true if the request was successfully processed
     *
     */
    bool postAdhocLabelChange(unsigned long messageKey, const std::string& label);

    /**
     * postAdhocTypeChange
     *
     * posts an adhoc type change notification to (via) the PA agent
     *
     * @param messageKey key identifying the message requiring update
     *
     * @param newMessageType the new type for the message
     *
     * @return true if the request was successfully processed
     *
     */
    bool postAdhocTypeChange(unsigned long messageKey, unsigned short newMessageType);

    /**
     * getCurrentPriorityEstimationMode
     *
     * @return the current priority estimation mode, for use in calculating
     *          what should be shown in the priority column of a PA Zone record
     *
     */
    int getCurrentPriorityEstimationMode();

    // All Broadcast parameters are thread guarded
    void setChime(bool chime);
	void setStisBroadcast(bool stisBroadcast);
	void setMultiMsgBroadcast(bool multiMsgBroadcast);//added by lixiaoxia for Multi-Messages function
	void setDwell(time_t dwell);
	void setPeriod(const time_t period);
    time_t getPeriod();


    /**
     * getStart
     *
     * @return the start time (as date) - in seconds from 1970 epoch
     *
     */
    time_t getStart();

    /**
     * getStop
     *
     * @return the stop time (as date) - in seconds from 1970 epoch
     *
     */
    time_t getStop();

    void setCyclicalEnabled(bool enabled);
    bool getCyclicalEnabled();

    void setCycSun(bool bEnable);
    bool getCycSun();
    void setCycMon(bool bEnable);
    bool getCycMon();
    void setCycTue(bool bEnable);
    bool getCycTue();
    void setCycWed(bool bEnable);
    bool getCycWed();
    void setCycThu(bool bEnable);
    bool getCycThu();
    void setCycFri(bool bEnable);
    bool getCycFri();
    void setCycSat(bool bEnable);
    bool getCycSat();

    /**
     * setSelectedMessages
     *
     * Updates the complete selected message set for this model
     *
     * @param newSelectedMessages the new set of selected messages
     *
     * @param updateView set to true to invalidate the associated view
     *          (force a refresh to reflect the changes)
     *
     */
    void setSelectedMessages(const std::list<unsigned long>& newSelectedMessages, bool updateView = false);
    
    /**
     * switchSelectedMsgKey
     *
     * switch two item values in m_selectedMessages.
     *
     * @param ulDragMsgKey  Drag item value(Message Key)       
     *
     * @param ulDropMsgKey  Drop item value(Message Key)
     *
     */
    void switchSelectedMsgKey(const unsigned int nDragOrder, const unsigned int unDropOrder);
    
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
     * If setting to Music, need to use other function with additional parameter
     *
     * @pre broadcastType is not StationMusic (use extended version of this function instead)
     *
     * @param broadcastType new type of broadcast
     *
     */
	void setBroadcastType(TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType broadcastType);


    /**
     * validateNewAdhocLabel
     *
     * Called to validate label changes on ad hoc list control
     *
     * @param text [IN/OUT] new string for label - 
     *                  character string may be modified (truncated) if too long
     *
     */
    void validateNewAdhocLabel( std::string& strNewLabel );


    /**
     * getZonesAssociatedWithGroup
     * 
     * @return an array of keys defining the zones associated with the specified zone group
     *      (at our current location)
     *
     * @param label the label of the zone group 
     *
     * @exception  DatabaseException 
     *          - if there are any problems in communicating with the database.
     *           
     * @exception  DataException 
     *          - if there is no PaZoneGroup matching the supplied parameters
     *
     */
    Keys getZonesAssociatedWithGroup(const std::string& label);

    Keys getSelectedZones();

    /**
     * removeSelectedMessagesWithKey 
     *
     * Clears from current selection all keys matching input key
     * and updates the view accordingly
     *
     * @param key the identifier defining which messages to remove
     *
     * @pre the input key corresponds to at least one of currently 
     *      selected keys
     *
     */
    void removeSelectedMessagesWithKey(unsigned long key);

    /**
     * executeMusic
     * 
     * Executes music broadcast via the PA Agent
     *
     * @param musicSource defines the source of the music (can be OCC or Station)
     *
     * @exception BadParameterException (passed from the PAAgent)
     *
     * @exception AccessDeniedException (from PA Agent - insufficient rights)
     *
     * @exception ValueNotSetException if the user hasn't set the music source
     *
     */
    void executeMusic(const TA_Base_Bus::IPAAgentCorbaDef::EMusicType& musicSource);

    /**
     * validateAndUpdateStartAndStop
     *
     * Updates the start and stop times for this class, during validation, it
     *  forces the start time to be after the current system time, and the
     *  stop time to be after the start time - these adjustments are automatically
     *  applied as applicable before updating internal state
     *
     * @pre the start and stop time have a date (year/day) component - they cannot
     *      be representative of a time of day only (because they're compared
     *      againt the current date)
     *
     */
    void validateAndUpdateStartAndStop(const time_t tmStart, const time_t tmStop);

protected:

    TA_IRS_Bus::StationDvaMessageRecords getNormalMessages(const std::string filterKeyword);
    TA_IRS_Bus::StationDvaMessageRecords getEmergencyMessages(const std::string filterKeyword);
    TA_IRS_Bus::StationDvaMessageRecords getAdhocMessages();
    std::list<unsigned long> getSelectedMessages();

    /**
     * getSelectedMessageFlags
     *
     * Retrieves a set of bit flags representing the kind of messages present
     *  in the selected messages array
     *
     * @return for each message in the selected messages array, perform an
     *      OR on the result with the corresponding value contained in EMessageType
     *      - Function will return 0 if no messages in selected messages array
     *
     */
    DWORD   getSelectedMessageFlags();

    /**
     * executeDVA
     * 
     * executes a DVA play request using the current parameters
     * 
     * @exception BadParameterException if there is something wrong with the parameters
     *              and the operation failed
     *
     * @exception AccessDeniedException (from PA Agent - insufficient rights)
     *
     */
    void executeDVA();

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
     * setStart
     *
     * Sets the starting time (as date)
     *
     * @param start time in seconds from 1970 epoch
     *
     */
    void setStart(time_t start);
    
    /**
     * setStop
     *
     * Sets the stopping time (as date)
     *
     * @param start time in seconds from 1970 epoch
     *
     */
    void setStop(time_t start);

    void updatePriority();
    void updateZoneList();

    void debugExecuteDVA();
    void debugExecuteLive();
    void debugExecuteRecording();

private:
    
    // Broad classification of message types
    enum EMessageType
    {
        EMERGENCY_PRERECORDED   = 1,
        EMERGENCY_ADHOC         = 2,
        NORMAL_PRERECORDED      = 4,
        NORMAL_ADHOC            = 8
    };

    IStationBroadcastPage&                      m_stationBroadcastPage;

    TA_Base_Core::PrimitiveWrapper<TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType>    m_broadcastType;
    Keys m_selectedZones;
    std::string m_selectedZoneGroupName;
    std::list<unsigned long> m_selectedMessages;
    time_t m_dwellInSecs;
    time_t m_startTime;
    time_t m_stopTime;
    time_t m_tmPeriod;
    bool m_isSynchronisedWithTIS;
	bool m_isMultiMsgBroadcast;//added by lixiaoxia for Multi-Messages function
    bool m_hasChime;
    bool m_cyclicalEnabled;
    bool m_bCycSun;
    bool m_bCycMon;
    bool m_bCycTue;
    bool m_bCycWed;
    bool m_bCycThu;
    bool m_bCycFri;
    bool m_bCycSat;

    // Stores the currently broadcasting music status (off if nothing being broadcast)
    TA_Base_Bus::IPAAgentCorbaDef::EMusicType    m_broadcastMusicSource;

	TA_Base_Core::NonReEntrantThreadLockable         m_lock;
};

#endif // !defined(AFX_StationBroadcastModel_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
