/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/RadioResourceButtonStrategy.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef RADIO_RESOURCE_BUTTON_STRATEGY_H_INCLUDED
#define RADIO_RESOURCE_BUTTON_STRATEGY_H_INCLUDED


#include "bus/radio/radio_bus/src/RadioTypes.h" 

#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/ButtonDecoratorType.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/IResourceButtonStrategy.h"


//Forward declaration;
class CResourceButton;

class RadioResourceButtonStrategy : public IResourceButtonStrategy
{

	public:
		RadioResourceButtonStrategy();

		virtual ~RadioResourceButtonStrategy();

	public:

		//A Group: these 18 methods from IResourceButtonStrategy(which is a "friend" to CResourceButton);

		virtual void initialise(CResourceButton* pResourceButton);

		virtual void selectionBehaviour();

		virtual void unselectionBehaviour();

		virtual void inboundCallRinging();

		virtual void inboundCall();

		virtual void inboundCallCanceled();

		virtual void selectSpeakerSelected();

		virtual void selectSpeakerUnselected();

		virtual void unselectSpeakerDeAssignment();

		virtual void resourceActivity();

		virtual void resourceActivityEnded();

		virtual void resourceEmergency();

		virtual void resourceEmergencyAcknowledged();

		virtual void resourceEmergencyEnded();

		virtual void reDraw();

		virtual bool canDropTarget();
		/**
		 * The following methods provide some common behavior;
		 */
		virtual int assignmentBehaviour(bool pOnInit);
		virtual void resourceAssignment();
		virtual void resourceDeAssignment();

		//B Group: these 3 methods from IDecoratorButtonBehaviourStrategy
		virtual void dropBehaviour(HDROP hDropInfo);

		virtual void leftClickBehaviour();

		virtual void rightClickBehaviour();

		virtual bool removeAssignedResource();
		
	protected:
		/**
		 * This method provides the common button decorations that all
		 * CRadioResourceButtons must call.
		 */
		void commonDecoration();
		/**
		 * Apply the according to the decorator type and the Resource's current state
		 */
		void applyDecorator(ButtonDecoratorType type);

		/*
		 * Note: Modify accessor to public 
		 */
		//virtual void removeAssignedResource();

		/**
		 * Returns the applicable bitmap/icon id for the RadioResourceType.
		 * The original return type is: LPCTSTR;
		 */
		int getBitmapForType(RadioResourceType type);


};
#endif


