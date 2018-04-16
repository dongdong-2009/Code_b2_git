/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/EmptyButtonStrategy.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef EMPTY_BUTTON_STRATEGY_H_INCLUDED
#define EMPTY_BUTTON_STRATEGY_H_INCLUDED

#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/IResourceButtonStrategy.h"

//Forward declaration;
class CResourceButton;

class EmptyButtonStrategy : public IResourceButtonStrategy
{

	public:

		EmptyButtonStrategy();

		virtual ~EmptyButtonStrategy();

	public:

		//A Group: these 18 methods from IResourceButtonStrategy;

		/**
		 * Initialise the button.
		 */
		virtual void initialise(CResourceButton* presourceButton);
		/**
		 * Check Rights for this operation.
		 */
		virtual bool canDropTarget();

		virtual void selectionBehaviour() { };

		virtual void unselectionBehaviour() {};

		virtual void inboundCallRinging() {};

		virtual void inboundCall() {};

		virtual void inboundCallCanceled() {};

		virtual void resourceActivity() {};

		virtual void resourceEmergency() {};


		/**
		 * Clear and re-apply the existing decorators;
		 */
		virtual void reDraw();


		virtual void selectSpeakerSelected() {};

		virtual void selectSpeakerUnselected() {};

		virtual void resourceActivityEnded() {};

		virtual void resourceEmergencyEnded() {};

		virtual void resourceEmergencyAcknowledged() {};

		virtual void unselectSpeakerDeAssignment() {};


		// IDecoratorButtonBehaviourStrategy
		virtual void dropBehaviour(HDROP hDropInfo);


		virtual void leftClickBehaviour() {};

		virtual void rightClickBehaviour() {};
	
		virtual int assignmentBehaviour(bool pOnInit) {  return 0; };

		virtual void resourceDeAssignment() {};

		virtual void resourceAssignment();

        virtual bool removeAssignedResource() {return true;};

	private:

		static const int  INT_PROFILE_BUTTON;
};
#endif