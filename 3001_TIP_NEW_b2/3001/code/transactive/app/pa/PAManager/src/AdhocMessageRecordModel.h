/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/AdhocMessageRecordModel.h $
  * @author Jade Lee
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2013/09/30 16:53:25 $
  * Last modified by: $Author: qi.huang $
  * 
  * Class to model the process of recording an ad-hoc message
  *
  */

#if !defined(AFX_ADHOCMESSAGERECORDMODEL_H__C1D937B1_69A0_4AC9_995C_28AA8183DB80__INCLUDED_)
#define AFX_ADHOCMESSAGERECORDMODEL_H__C1D937B1_69A0_4AC9_995C_28AA8183DB80__INCLUDED_

#include "app/pa/PAManager/src/IAdhocMessageRecordPage.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"

class PaProgressReportSubscriber;
class IWindow;
class IAdhocMessageRecordPage;

const char* const s_pressPttPrompt = "Press PTT on the PA paging console before speaking.";
const char* const s_releasePttPrompt = "Release PTT on the PA paging console to terminate.";
const char* const s_recordingComplete = "Ad-hoc recording completed successfully.";
const char* const s_recordingFailed = "Ad-hoc recording failed.";
const char* const s_recordingAborted = "Ad-hoc recording is stopped/finished.";

class AdhocMessageRecordModel
{
public:

    /**
     * AdhocMessageRecordModel (constructor)
     *
     * @param pKey the primary key defininf the ad-hoc message
     *          we're recording over
     *
     */
    AdhocMessageRecordModel(const unsigned long pKey, IAdhocMessageRecordPage& adhocMessageRecordPage);

    virtual ~AdhocMessageRecordModel();

    
    /**
     * processRecordAdhocNotification
     *
     * The owner window calls this function whenever it gets a progress report
     *  updates (subscribed to by this model)
     *     
     * @return true if the progress report indicated recording is complete, and
     *          the model has destroyed the recording broadcast and subscriber (cleaned up)
     *
     * @param broadcastState [OUT] (optional) current state of broadcast,
     *                   or unset if broadcast no longer exists
     *
     */
    bool processRecordAdhocNotification(UINT wParam, bool& refStartTiming,
          TA_Base_Core::PrimitiveWrapper<TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState>* broadcastState = 0);

    /**
     * isRecording
     *
     * @return true if the model is currently in process of recording a message
     *
     */
    bool isRecording() const;

    void setEmergencyMsg( const bool bEmergency );

    bool isEmergencyMsg();

    /**
     * beginRecording
     *
     * Begins recording an ad-hoc message over the ad-hoc message
     *  defined by local message primary key (set in constructor)
     * The user must call stopRecording before
     *  the window defined by notificationWnd is destroyed
     *
     * @pre the model is not currently recording a message.
     *
     * @param notificationWnd the window to receive progress report
     *          update notifications (which should be passed on to model)
     *
     * @exception BadParameterException invalid parameters for request
     *
     * @exception AccessDeniedException (from PA Agent - insufficient rights)
     *
     * @exception CORBA::Exception comms error
     *
     * @exception ObjectResolutionException comms error
     *
     */
    void beginRecording(IWindow& notificationWnd);

    /**
     * stopRecording
     *
     * Stops the recording of the adhoc message that was previously started with
     *  a call to beginRecording
     *
     * @pre isRecording() returns true
     *
     * @return false if there was an error terminating the recording via the PA
     *      agent (but the broadcast ID will still be removed from this class),
     *      and it will consider itself to no longer be in a recording state
     *
     */
    bool stopRecording();

    /**
     * requestTermination
     *
     * Forwards a request to the agent to terminate this recording
     *  The broadcast should promptly enter a PendingRemoval state
     *
     * @exception CORBA::Exception on comms error
     * @exception TransactiveException on unexpected error
     *
     */
    void requestTermination();

    /**
     * getMessageKey (simple accessor)
     *
     */
    unsigned long getMessageKey() const { return m_adhocMessageKey; }

	const char* getBroadcastId();

    bool isRecordingFailed();

    bool isPttActivated();

private:
    unsigned long m_adhocMessageKey;
    bool m_bEmergencyMsg;

    // To listen out for recording progress
    PaProgressReportSubscriber* m_progressReportSubscriber;

    IAdhocMessageRecordPage& m_adhocMessageRecordPage;

    bool m_bRuntimeAcked;
    bool m_bPttActiveAcked;

    bool m_bRecordingFailed;
};

#endif // AFX_ADHOCMESSAGERECORDMODEL_H__C1D937B1_69A0_4AC9_995C_28AA8183DB80__INCLUDED_
