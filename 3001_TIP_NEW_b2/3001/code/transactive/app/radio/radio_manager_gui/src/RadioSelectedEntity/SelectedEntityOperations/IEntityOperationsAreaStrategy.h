/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/IEntityOperationsAreaStrategy.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * This is the common interface for the CEntityOperationsAreaDlg strategy. These
 * strategies will define the behaviour of the CEntityOperationsAreaDlg for
 * different Resource types. The only operation thas has to be explicitly
 * implemented on all strategies is the Initialise operation. All other operations
 * can either be overloaded with an empty stub, or the
 * IEntityOperationsAreaStrategy method itself may be stubbed.
 */

#if !defined(AFX_IENTITYOPERATIONSAREASTRATEGY_H__D2FC71F0_C4F0_4101_B0B3_6CF983FD337A__INCLUDED_)
#define AFX_IENTITYOPERATIONSAREASTRATEGY_H__D2FC71F0_C4F0_4101_B0B3_6CF983FD337A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "bus/radio/radio_bus/src/RadioTypes.h"

class CEntityOperationsAreaDlg;

class IEntityOperationsAreaStrategy  
{
	public:

		virtual ~IEntityOperationsAreaStrategy() { };

	public:
		/**
		 * Initialise the dialog by showing/hiding and populating any controls based on
		 * the entity Type.
		 */
		virtual void initialise(CEntityOperationsAreaDlg* pentityOperationsDlg) =0;
		/**
		 * Perform the Select Speaker function of the Resource type
		 */
		virtual void selectSpeaker() =0;
		/**
		 * Perform the Unselect Speaker function of the Resource type
		 */
		virtual void unselectSpeaker() =0;        
		/**
		 * Perform the Adjust Volume function of the Resource type
		 */
		virtual void adjustVolume() =0;
		/**
		 * Perform the Call function of the Resource type
		 */
		virtual void call() =0;
		/**
		 * Perform the Answer function of the Resource type
		 */
		virtual void answer() =0;
		/**
		 * Perform the End call function of the Resource type
		 */
		virtual void endCall() =0;
		/**
		 * Perform the Send Text... function of the Resource type
		 */
		//virtual void sendText() =0;
		/**
		 * Perform the Patch... function of the Resource type
		 */
		virtual void patch() =0;
		/**
		 * Perform the Ambience Listening function of the Resource type
		 */
		virtual void ambienceListening() =0;
		/**
		 * Perform the Connect function of the Resource type
		 */
		virtual void connect() =0;
		/**
		 * Perform the Release function of the Resource type
		 */
		virtual void release() =0;


		/**
		 * Update the buttons states when there is a change in connection state
         * i.e., CONNECTED / RELEASED
		 */
        virtual void onConnectState(GroupConnectionState pState) {};

	public:
		
		CEntityOperationsAreaDlg *m_CEntityOperationsAreaDlg;
};

#endif // !defined(AFX_IENTITYOPERATIONSAREASTRATEGY_H__D2FC71F0_C4F0_4101_B0B3_6CF983FD337A__INCLUDED_)
