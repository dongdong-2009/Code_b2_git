/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/RadioEntityOperationsAreaStrategy.h $
  * @author:  F. Stuart
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */

#if !defined(AFX_RADIOENTITYOPERATIONSAREASTRATEGY_H__DEEE4BE9_1409_46EF_AFE8_C8DFA65C3825__INCLUDED_)
#define AFX_RADIOENTITYOPERATIONSAREASTRATEGY_H__DEEE4BE9_1409_46EF_AFE8_C8DFA65C3825__INCLUDED_


#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/IEntityOperationsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/CEntityOperationsAreaDlg.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RadioEntityOperationsAreaStrategy : public IEntityOperationsAreaStrategy 
{
	public:
		RadioEntityOperationsAreaStrategy();

		virtual ~RadioEntityOperationsAreaStrategy();

	public:
		/**
		 * Initialise the dialog by showing/hiding and populating any controls based on
		 * the entity Type.
		 */
		virtual void initialise(CEntityOperationsAreaDlg* pEntityOperationsDlg);
		/**
		 * Perform the Connect function of the Resource type
		 */
		virtual void connect() {};
		/**
		 * Perform the Select Speaker function of the Resource type
		 */
		virtual void selectSpeaker() {};
		/**
		 * Perform the Unselect Speaker function of the Resource type
		 */
		virtual void unselectSpeaker() {};
		/**
		 * Perform the Call function of the Resource type
		 */
		virtual void call();
		/**
		 * Perform the Answer function of the Resource type
		 */
		virtual void answer();
		/**
		 * Perform the End call function of the Resource type
		 */
		virtual void endCall();
		/**
		 * Perform the Send Text... function of the Resource type
		 */
		//virtual void sendText();
		/**
		 * Perform the Patch... function of the Resource type
		 */
		virtual void patch();
		/**
		 * Perform the Ambience Listening function of the Resource type
		 */
		virtual void ambienceListening();
		/**
		 * Perform the Adjust Volume function of the Resource type
		 */
		virtual void adjustVolume();
		/**
		 * Perform the Release function of the Resource type
		 */
		virtual void release()  {};

	private:

		/*
		 * Checks whether the operator has privileges to make an outgoing CALL
		 * or ANSWER an incoming call
		 */
		void checkControlRights();

		/* 
		 * update the controls state according to the current private call state.
		 */
		void UpdateControlState();
};

#endif // !defined(AFX_RADIOENTITYOPERATIONSAREASTRATEGY_H__DEEE4BE9_1409_46EF_AFE8_C8DFA65C3825__INCLUDED_)
