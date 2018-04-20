/*
 * The soiurce code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/DirectLineCache.h $
 * @author:  Andy Parker
 * @version: $Revision: #1 $
 *
 * Last mofification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This class manages the set of direct lines used by the application.  It 
 * is accessed by the direct line property page and direct line configuration
 * property page
 *
 */
#if !defined(AFX_DIRECTLINECACHE_H__74B1B803_632A_11D7_B162_0050BAB1D931__INCLUDED_)
#define AFX_DIRECTLINECACHE_H__74B1B803_632A_11D7_B162_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "app/radio/radio_manager_gui/src/RadioDirectLines.h"
#include "core\message\src\SpecialisedMessageSubscriber.h"
#include "CommsMessageCorbaDef.h"
#include "bus\radio\RadioAgentCorbaDef_4669\idl\src\IRadioCorbaDef.h"


class CDirectLineCache 
:
public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
{
    public:

	    CDirectLineCache();
	    virtual ~CDirectLineCache();

		void SetUsername(CString userName);

        UINT    GetButtonSetCount();
		CString GetButtonSetName(UINT iSet);

		UINT    GetButtonsPerSet(UINT buttonSet);
		CString GetButtonLabel(UINT buttonSet, UINT buttonNumber);
		void	SetButtonLabel(UINT buttonSet,UINT buttonNumber, std::string label);
		CString GetButtonNumber(UINT buttonSet, UINT buttonNumber);
		void    SetButtonNumber(UINT buttonSet, UINT buttonNumber, std::string number);

		int     GetHotButtonsInSet(void);
		CString GetHotButtonLabel(UINT buttonNumber);
		CString GetHotButtonNumber(UINT buttonNumber);
		void    SetHotButtonLabel(UINT buttonNumber, std::string label);
		void	SetHotButtonNumber(UINT buttonNumber, std::string number);


		bool ReloadCache();
        bool UpdateButton(UINT buttonSet,UINT buttonNumber, CString label, CString number);

        void ApplyButtonChanges(UINT buttonSet, UINT buttonNumber);
        void ApplyHotButtonChanges(UINT buttonNumber);

        // Set up so that the cache has a valid set of data
        void InitData(const std::string& radioName);
        // Unsubscribe from messages etc
        void UnInitData();

        /// SpecialisedMessageSubscriber pure virtual implementation
        virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

        /// Called just prior to refreshing the GUI, may prompt the cache to be
        /// reloaded
        void onRefreshNotificationReceived();

		// Singleton access
		static CDirectLineCache*  GetInstance() {return &CDirectLineCache::s_Instance;};

	private:

        /// Processes subscriber updates
        void processUpdate(const TA_Base_Bus::IRadioCorbaDef::SpeedDialUpdateNotification& update);

        void changeButtonLineDetails(CRadioDirectLines::DirectLineVector& buttonLine,UINT buttonNumber,
                                               const CString& buttonSetName);

		CRadioDirectLines						m_radioDirectLines;
		CRadioDirectLines::DirectLineVector     m_hotLines;
		CRadioDirectLines::DirectLineVector     m_buttonLines[5];
		CString									m_buttonSets[5];

//		CRadioSession*							m_radioSession;
		CString									m_userName;

		static CDirectLineCache					s_Instance;   // The singleton instance

        static const CString                    HOT_LINE_SET_NAME;
        
        TA_Base_Core::ReEntrantThreadLockable  m_commsLock;
        bool    m_cacheReloadRequired;
        bool    m_processSubscribedMessages;
};

#endif // !defined(AFX_DIRECTLINECACHE_H__74B1B803_632A_11D7_B162_0050BAB1D931__INCLUDED_)
