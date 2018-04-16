/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/CEntityOperationsAreaDlg.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef ENTITY_OPERATIONS_AREA_DLG_H_INCLUDED
#define ENTITY_OPERATIONS_AREA_DLG_H_INCLUDED


#include "app/radio/radio_manager_gui_icdv4/src/RadioUserDefinedEvents.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"


//Forward declaration;
class IEntityOperationsAreaStrategy;

/////////////////////////////////////////////////////////////////////////////
// CEntityOperationsAreaDlg dialog

class CEntityOperationsAreaDlg : public CDialog
{
	friend class IEntityOperationsAreaStrategy;
	friend class EmptyEntityOperationsAreaStrategy;
	friend class DynamicRegroupEntityOperationsAreaStrategy;
	friend class MultiselectEntityOperationsAreaStrategy;
	friend class MultiSiteOperationsAreaStrategy;
	friend class PatchGroupEntityOperationsAreaStrategy;
	friend class RadioEntityOperationsAreaStrategy;
	friend class TalkGroupEntityOperationsAreaStrategy;
	friend class SiteEntityOperationsAreaStrategy;
	friend class AnnouncementGroupEntityOperationsAreaStrategy;

	public:

		CEntityOperationsAreaDlg(CWnd* pParent = NULL);
		virtual ~CEntityOperationsAreaDlg();

    // Dialog Data
	    //{{AFX_DATA(CEntityOperationsAreaDlg)
		enum { IDD = IDD_SELECTED_OPERATIONS };
		CStatic	m_staticText;
		CSliderCtrl	m_volumeSlider;
		CButton	m_callButton;
		CButton	m_answerButton;
		CButton	m_endCallButton;
		CButton	m_connectButton;
		CButton	m_selectedSpeakerButton;
		CButton	m_releaseButton;
		CButton	m_sendTextButton;
		CButton	m_patchButton;
		CButton	m_ambienceButton;
	//}}AFX_DATA

    // Overrides
	    //{{AFX_VIRTUAL(CEntityOperationsAreaDlg)
	    protected:
            virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

	public:
		/**
		 * If no RadioResource, display the default "empty" dialog.
		 */
		void initialise();
        RadioResource& getCurrentResource();


    // Implementation
	protected:

        //{{AFX_MSG(CEntityOperationsAreaDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnReleasedCaptureVolumeSlider(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnAmbienceListening();
        afx_msg void OnAnswer();
        afx_msg void OnCall();
        afx_msg void OnConnect();
        afx_msg void OnEndCall();
        afx_msg void OnPatch();
        afx_msg void OnRelease();
        afx_msg void OnSendText();
        afx_msg void OnSpeaker();

        afx_msg LRESULT OnNotifiedEntitySelectionChanged(WPARAM pRadioResource, LPARAM unused);
		afx_msg LRESULT OnNotifiedEditMode(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnNotifiedOperationMode(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnNotifiedVolumeChanged(WPARAM pvolumeChangeData, LPARAM volume);
		afx_msg LRESULT OnNotifiedSelectSpeakerSelected(WPARAM pselectSpeakerData, LPARAM unused);
		afx_msg LRESULT OnNotifiedSelectSpeakerUnselected(WPARAM pselectSpeakerData, LPARAM unused);
		afx_msg LRESULT OnNotifiedEntityBeingCreated(WPARAM pselectSpeakerData, LPARAM unused);
		afx_msg LRESULT OnNotifiedEntityUpdated(WPARAM resource, LPARAM unused);
		afx_msg LRESULT OnNotifiedConnectGroup(WPARAM resource, LPARAM pData);
		afx_msg LRESULT OnNotifiedPrivateCallEvent(WPARAM resource, LPARAM pData); 
		afx_msg LRESULT onNotifiedEntityEmptySelection(WPARAM pRadioResourceType, LPARAM unused);
	//}}AFX_MSG
    	DECLARE_MESSAGE_MAP()

	private:

		volatile IEntityOperationsAreaStrategy* m_pEntityOperationsAreaStrategy;

//TD18785
		void checkConnectGroup(RadioResource &, GroupConnectionState);
		void checkPrivateCall(RadioResource &);
//TD18785
		bool m_bEditMode;

	protected:

		RadioResource m_RadioResource;

};

#endif
