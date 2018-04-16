/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/CResourceButton.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * This class in intended to be implemented with the Checkbox button style.

 */

#ifndef RESOURCE_BUTTON_H_INCLUDED
#define RESOURCE_BUTTON_H_INCLUDED


#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "bus/mfc_extensions/src/decorator_button/CDecoratorButton.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/ButtonDecoratorType.h"

#include <set>


//Forward declaration;
class IResourceButtonStrategy;
class IResourceButtonContainer;

class CResourceButton : public CDecoratorButton
{

	friend class RadioResourceButtonStrategy;
	friend class AnnouncementGroupRadioResourceButtonStrategy;
	friend class DynamicRegroupRadioResourceButtonStrategy;
	friend class EmptyButtonStrategy;
	friend class MultiSelectRadioResourceButtonStrategy;
	friend class MultiSiteRadioResourceButtonStrategy;
	friend class PatchGroupRadioResourceButtonStrategy;
	friend class RadioRadioResourceButtonStrategy;
	friend class SiteRadioResourceButtonStrategy;
	friend class TalkGroupRadioResourceButtonStrategy;
	friend class RadioMonitorButtonStrategy;
		

	public:
		
		CResourceButton();

		virtual ~CResourceButton();

		void setResourceButtonStrategy(IResourceButtonStrategy* pStrategy);
		IResourceButtonStrategy* getResourceButtonStrategy();

		// NOTE: This functionality is implemented by OnDropResource()
		//afx_msg void OnDropFiles(HDROP hDropInfo);

	    /**
	      * CResourceButton*
	      * 
	      * Returns a reference to the undecorated concrete resource button(pointer or reference);
	      * 
	      * @return void 
	      */
		virtual CResourceButton* undecoratedResourceButton();

		void setRadioResource(RadioResource& resource);

		RadioResource getRadioResource();
		
		void setResourceButtonContainer(IResourceButtonContainer* pButtonContainer);

		/**
	      * setProfileButton
	      * 
	      * Method called when user assigns this button to a resource on 
	      * drag 'n drop operation
		  *
	      * @return void 
	      */
		void OnDropResource(RadioResource& resource);

	    /**
	      * setProfileButton
	      * 
	      * Method to set whether this button is profile/user-defined
	      * 
	      * @return void 
	      */
		void setProfileButton(int nProfile);

	    /**
	      * assignButtonOnInit
	      * 
	      * Methods to access reference# of this button instance
	      * 
	      * @return void 
	      */
		void setButtonReference(unsigned long buttonRef);
		unsigned long getButtonReference();
		
	    /**
	      * assignButtonOnInit
	      * 
	      * Initializes the button during startup when AP is in operation mode
	      * 
	      * @return void 
	      */
		void assignButtonOnInit(RadioResource& resource);

		void stopMonitor();

		void cancelStopMonitor();

		static const long RADIO_BUTTON_REFERENCE;

	    virtual void setCheck(bool checkIt);

		void setPressInProcess();

		bool isChanged() {return m_isChanged;}

		
	public: 

		//{{AFX_MSG(CResourceButton)
		afx_msg LRESULT onNotifiedEditMode(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedOperationMode(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceAssignment(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceDeAssignment(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedSelectSpeakerSelected(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedSelectSpeakerUnselected(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedInboundCall(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedInboundCallCanceled(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceActivity(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceActivityEnded(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceEmergency(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceEmergencyAcknowledged(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceEmergencyEnded(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedTimerTriggered(WPARAM wParam, LPARAM lParam);
		//afx_msg LRESULT onNotifiedPrivateCallEstablished(WPARAM wParam, LPARAM lParam);
		//afx_msg LRESULT onNotifiedPrivateCallRinging(WPARAM wParam, LPARAM lParam);
		//afx_msg LRESULT onNotifiedPrivateCallEnded(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedOperationFail(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedProfileUpdate(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedDbUpdate(WPARAM pOldResource = 0, LPARAM pNewResource = 0);
		afx_msg LRESULT onNotifiedDbDelete(WPARAM pOldResource = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedApiFailureMode(WPARAM connected = 0, LPARAM lParam = 0);

		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);		
		afx_msg void OnRButtonUp(UINT nFlags, CPoint point);		
		afx_msg void OnStopMonitorMenu();
		afx_msg void OnAcknowledgeEmergencyMenu();
		afx_msg void OnKnockdownEmergencyMenu();
		afx_msg void OnStartEmergencyMenu();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()


	private:

			/**
			 * registers for user-defined messages
			 */
			void addObservers();
			
			void removeObservers();

			
	// Attributes
	private:
		/**
		 * RadioResource is a struct defined in RadioTypes.h;
		 */
		RadioResource m_RadioResource;
		/**
		 * previous RadioResource state
		 */
		RadioResource m_PrevRadioResource;
		/**
		 * m_pResourceButtonStrategy may change asynchronously;
		 */
		volatile IResourceButtonStrategy* m_pResourceButtonStrategy;
		/**
		 * Indicates if this button is a profile controlled button.
		 *  1 <-> profile-based
		 *  0 <-> user-defined
		 */
		int m_isProfileButton;

		IResourceButtonContainer* m_pResourceButtonContainer;

		/*
		 * Button reference
		 */
		unsigned long m_buttonRef;


	protected:
		/**
		 * This collection will be useful when just re-applying existing decorators for a
		 * timer-callback. Using a std::set to  group in sorted order. The values are
		 * enumerations so the default predicate of std::less can be used.
		 */
		std::set<ButtonDecoratorType> m_appliedDecoratorTypesCollection;
		/**
		 * Reflects if the flashing Timer's trigger is in "On" or "Off"
		 */
		bool m_isFlashOff;
		bool m_isAckEmergency;
        bool m_isActive;

		bool m_isPrevFlashOff;
		bool m_isPrevAckEmergency;
		bool m_isPrevActive;

		// flag to indicate whether predefined TG is already changed
		bool m_isChanged;
};
#endif
