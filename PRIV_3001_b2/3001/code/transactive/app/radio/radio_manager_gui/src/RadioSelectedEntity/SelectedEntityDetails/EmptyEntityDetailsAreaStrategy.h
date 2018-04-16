/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/EmptyEntityDetailsAreaStrategy.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef EMPTY_ENTITY_DETAILS_AREA_STRATEGY_H_INCLUDED
#define EMPTY_ENTITY_DETAILS_AREA_STRATEGY_H_INCLUDED


#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/IEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/CEntityDetailsAreaDlg.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

class EmptyEntityDetailsAreaStrategy : public IEntityDetailsAreaStrategy
{

public:
	EmptyEntityDetailsAreaStrategy();
	virtual ~EmptyEntityDetailsAreaStrategy();

public:
	/**
	 * Initialise the dialog by showing/hiding and populating any controls based on
	 * the entity Type.
	 */
	virtual void initialise(CEntityDetailsAreaDlg* pentityDetailsArea);
	/**
	 * Perform the Delete function of the Resource type. 
	 */
	virtual void deleteResource(){};
	/**
	 * Perform the Edit function of the Resource type.
	 */
	virtual void edit(){};
	/**
	 * Perform the Save function of the Resource type.
	 */
	virtual void save(){};
	/**
	 * Perform the Cancel function of the Resource type.
	 */
	virtual void cancel(){};
	/**
	 * Perform the Drop Files function of the Resource type.
	 */
	virtual void OnDropFiles(RadioResource& resource) {};

	private:
        //disable method
        EmptyEntityDetailsAreaStrategy (EmptyEntityDetailsAreaStrategy& radioObj);
        void operator=(const EmptyEntityDetailsAreaStrategy& radioObj);


};
#endif