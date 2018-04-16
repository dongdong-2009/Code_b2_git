/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/TalkGroupEntityOperationsAreaStrategy.h $
  * @author:  F. Stuart
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */


#if !defined(AFX_TALKGROUPENTITYOPERATIONSAREASTRATEGY_H__645235FB_F05B_48FF_A6D2_A0BD990D85AA__INCLUDED_)
#define AFX_TALKGROUPENTITYOPERATIONSAREASTRATEGY_H__645235FB_F05B_48FF_A6D2_A0BD990D85AA__INCLUDED_


#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/IEntityOperationsAreaStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/CEntityOperationsAreaDlg.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TalkGroupEntityOperationsAreaStrategy : public IEntityOperationsAreaStrategy 
{
	public:
		TalkGroupEntityOperationsAreaStrategy();
		
		virtual ~TalkGroupEntityOperationsAreaStrategy();

	public:
		/**
		 * Initialise the dialog by showing/hiding and populating any controls based on
		 * the entity Type.
		 */
		virtual void initialise(CEntityOperationsAreaDlg* pentityOperationsDlg);
		/**
		 * Perform the Adjust Volume function of the Resource type
		 */
		virtual void adjustVolume();
		/**
		 * Perform the Select Speaker function of the Resource type
		 */
		virtual void selectSpeaker();
		/**
		 * Perform the Unselect Speaker function of the Resource type
		 */
		virtual void unselectSpeaker();
		/**
		 * Perform the Send Text... function of the Resource type
		 */
		virtual void sendText();
		/**
		 * Perform the Patch... function of the Resource type
		 */
		virtual void patch();
		/**
		 * Perform the Call function of the Resource type
		 */
		virtual void call() {};
		/**
		 * Perform the Answer function of the Resource type
		 */
		virtual void answer() {};
		/**
		 * Perform the End call function of the Resource type
		 */
		virtual void endCall() {};
		/**
		 * Perform the Ambience Listening function of the Resource type
		 */
		virtual void ambienceListening() {};
		/**
		 * Perform the Connect function of the Resource type
		 */
		virtual void connect() {};
		/**
		 * Perform the Release function of the Resource type
		 */
		virtual void release() {};
};

#endif // !defined(AFX_TALKGROUPENTITYOPERATIONSAREASTRATEGY_H__645235FB_F05B_48FF_A6D2_A0BD990D85AA__INCLUDED_)
