/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/IResourceButtonStrategy.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef INTERFACE_RESOURCE_BUTTON_STRATEGY_H_INCLUDED
#define INTERFACE_RESOURCE_BUTTON_STRATEGY_H_INCLUDED

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/CResourceButton.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/ResourceButtonStrategyFactory.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/ButtonDecoratorType.h"

#include "bus/mfc_extensions/src/decorator_button/IDecoratorButtonBehaviourStrategy.h"

#include "core/utilities/src/TAAssert.h"

#include "core/utilities/src/DebugUtil.h"

#include <set>


class IResourceButtonStrategy : public TA_Base_Bus::IDecoratorButtonBehaviourStrategy
{

	public:
		/**
		 * Destructor;
		 */
		~IResourceButtonStrategy() {};
		/**
		 * Initialise the button, all the following methods define a complete protocol for those child classes;
		 */
		virtual void initialise(CResourceButton* pResourceButton) = 0;

		virtual void selectionBehaviour() = 0;

		virtual void unselectionBehaviour() = 0;

		virtual int assignmentBehaviour(bool pOnInit) = 0 ;

		virtual void resourceAssignment() = 0;

		virtual void resourceDeAssignment() = 0;

		virtual void inboundCallRinging() = 0;

		virtual void inboundCall() = 0;

		virtual void inboundCallCanceled() = 0;

		virtual void selectSpeakerSelected() = 0;

		virtual void selectSpeakerUnselected() = 0;

		virtual void unselectSpeakerDeAssignment() = 0;

		virtual void resourceActivity() = 0;

		virtual void resourceActivityEnded() = 0;

		virtual void resourceEmergency() = 0;

		virtual void resourceEmergencyAcknowledged() = 0;

		virtual void resourceEmergencyEnded() = 0;

		// Clear and re-apply the existing decorators.
		virtual void reDraw() = 0;

		virtual bool canDropTarget() = 0;

		//Inherited methods from IDecoratorButtonBehaviourStrategy

		virtual void leftClickBehaviour() = 0;

		virtual void rightClickBehaviour() = 0;

		virtual void dropBehaviour(HDROP hDropInfo) = 0 ;

        // called after right click to stop monitor
		virtual bool removeAssignedResource() = 0;

	public:

		CResourceButton *m_pResourceButton;

};

#endif