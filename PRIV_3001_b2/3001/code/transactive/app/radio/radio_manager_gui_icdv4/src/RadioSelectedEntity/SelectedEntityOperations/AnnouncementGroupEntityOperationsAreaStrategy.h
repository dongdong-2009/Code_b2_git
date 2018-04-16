/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/AnnouncementGroupEntityOperationsAreaStrategy.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef ANNOUNCEMENT_GROUP_ENTITY_OPERATIONS_AREA_STRATEGY_H_INCLUDED
#define ANNOUNCEMENT_GROUP_ENTITY_OPERATIONS_AREA_STRATEGY_H_INCLUDED

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/IEntityOperationsAreaStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/CEntityOperationsAreaDlg.h"

class AnnouncementGroupEntityOperationsAreaStrategy : public IEntityOperationsAreaStrategy
{

public:
	AnnouncementGroupEntityOperationsAreaStrategy();
	virtual ~AnnouncementGroupEntityOperationsAreaStrategy();

public:
	/**
	 * Initialise the dialog by showing/hiding and populating any controls based on
	 * the entity Type.
	 */
	virtual void initialise(CEntityOperationsAreaDlg* pentityOperationsDlg);
	/**
	 * Perform the Select Speaker Selected function of the Resource type
	 */
	virtual void selectSpeaker();
	/**
	 * Perform the Unselect Speaker Selected function of the Resource type
	 */
	virtual void unselectSpeaker();
	/**
	 * Perform the Adjust Volume function of the Resource type
	 */
	virtual void adjustVolume();
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
	 * Perform the Send Text... function of the Resource type
	 */
	virtual void sendText() {};
	/**
	 * Perform the Patch... function of the Resource type
	 */
	virtual void patch() {};
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
#endif 