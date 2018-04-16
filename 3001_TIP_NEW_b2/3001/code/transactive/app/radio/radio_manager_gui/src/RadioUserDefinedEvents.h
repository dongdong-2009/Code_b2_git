/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioUserDefinedEvents.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Defines the set of user-defined messages
 */

#if !defined(AFX_RADIOTYPES_H__7CF74587_3CFF_4ED7_9896_D96A662D6A40__INCLUDED_)
#define AFX_RADIOTYPES_H__7CF74587_3CFF_4ED7_9896_D96A662D6A40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



/**************************************************
/*  Indicates the type of update
/**************************************************/
enum RadioUpdateMessageType
{
	RM_NEW,
	RM_UPDATE,
	RM_DELETE
};

enum RadioButtonState
{
	BN_PRESSED,
	BN_UNPRESSED,
	BN_IN_PROCESS
};


/**************************************************
/*  Defines the set of user-defined events
/**************************************************/
enum RadioUserDefinedEvents
{
	/**
	 * The message to send when an entity is being edited.
	 */
	WM_ENTITY_EDIT_MODE = WM_APP+1,
	/**
	 * The message to send when an entity is being created.
	 */
	WM_ENTITY_BEING_CREATED,
	WM_ENTITY_UPDATED,
	/**
	 * The message to send when normal operation mode is entered. This would typically
	 * be called at the end of an entity edit or at initialization of the application.
	 */
	WM_OPERATION_MODE,
	/**
	 * The message to send when the current entity selection is changed.
	 */
	WM_ENTITY_SELECTION_CHANGED,
	/**
	 * The message to send when a resource assignment message have been received.
	 */
	WM_RESOURCE_ASSIGNMENT,
	/**
	 * The message to send when a resource de-assignment message have been received.
	 */
	WM_RESOURCE_DE_ASSIGNMENT,
	/**
	 * The message to send when the selected or unselected volume has changed.
	 */
	WM_VOLUME_CHANGED,
	/**
	 * The message to send when the select speaker selected message have been received.
	 */
	WM_SELECT_SPEAKER_SELECTED,
	/**
	 * The message to send when the select speaker unselected message have been
	 * received.
	 */
	WM_SELECT_SPEAKER_UNSELECTED,
	/**
	 * The message to send when a new inbound call message have been received.
	 */
	WM_INBOUND_CALL,
	/**
	 * The message to send when an inbound call canceled message have been received.
	 */
	WM_INBOUND_CALL_CANCELED,
	/**
	 * The message to send when a Private Call is ringing(INCOMING)
	 */
	WM_PRIVATE_CALL_RINGING,

	/**
	 * Fixed TD#14131
	 * The message to send when: 
	 * A request to initiate a private call is being processed
	 * The target unit of a private call request is "RINGING" (OUTGOING)
	 */
	 WM_PRIVATE_CALL_PROCESSED,

	/**
	 * The message to send when a Private Call have been established.
	 */
	WM_PRIVATE_CALL_ESTABLISHED,
	/**
	 * The message to send when a Private Call have been ended.
	 */
	WM_PRIVATE_CALL_ENDED,
	/**
	 * The message to send when an inbound PTT message have been received.
	 */
	WM_INBOUND_PTT,
	/**
	 * The message to send when an inbound PTT released message have been received.
	 */
	WM_INBOUND_PTT_RELEASED,
	/**
	 * The message to send when a Resource Activity message have been received.
	 */
	WM_RESOURCE_ACTIVITY,
	/**
	 * The message to send when a Resource Activity ended message have been received.
	 */
	WM_RESOURCE_ACTIVITY_ENDED,
	/**
	 * The message to send when a Resource Emergency message have been received.
	 */
	WM_RESOURCE_EMERGENCY,
	/**
	 * The message to send when a Resource Emergency Acknowledged message have been
	 * received.
	 */
	WM_RESOURCE_EMERGENCY_ACKNOWLEDGED,
	/**
	 * The message to send when a Resource Emergency Ended message have been received.
	 */
	WM_RESOURCE_EMERGENCY_ENDED,
	/**
	 * The message to send when a new predefined message have been added.
	 */
	WM_PREDEFINEDMSG_ADDED,
	/**
	 * The message to send when a predefined message have been MODIFIED.
	 */
	WM_PREDEFINEDMSG_MODIFIED,
	/**
	 * The message to send when a predefined message have been removed.
	 */
	WM_PREDEFINEDMSG_REMOVED,
	/**
	 * The message to send when a new text message have been received.
	 */
	WM_TEXTMSG_RECEIVED,
	/**
	 * The message to send when an Event Occurred message have been received.
	 */
	WM_AUDIT_EVENT_OCCURRED,
	/**
	 * The message to generate when a Timer timeout has occurred.
	 */
	WM_TIMER_TRIGGERED,

    WM_DD_DRAGDROP,  // drop
	/**
	 * The message to generate MFT Agent data are initialised
	 */
	WM_DATABASE_READY,
	
	WM_OUTGOING_CALL,

	WM_AMBIENCE_LISTENING,

	WM_CONNECT_GROUP,

	/**
	 * The message to send when a DLTS notification needs to be processed in Patch Operations
	 */
	WM_DLTS_CALL_ESTABLISHED,

	WM_DLTS_CALL_ON_HOLD,

	/**
	 * The message to send when TelephoneManager wants to launch PatchManager dialog
	 */
	WM_LAUNCH_PATCH_DLG,

	/* fixed TD14271
	 * The message to send when TelephoneManager are initialising the GUI and the APIs.
	 */
	WM_INITIALISE_MODE,

	WM_APIFAILURE_MODE,

//TD18869
	WM_ENTITY_BEING_DELETED,
//TD18869

	WM_ENTITY_EMPTY_SELECTION,

	WM_START_SUBS_MONITORING,

	WM_STOP_SUBS_MONITORING,

	WM_INCOMING_DRIVER_CALL,
	
	WM_ESTABLISHED_DRIVER_CALL,
	
	WM_OPERATION_FAIL,

	WM_DB_NEW,

	WM_DB_DELETE,
	
	WM_DB_UPDATE,

	WM_PROFILE_UPDATE,

	WM_DB_FINISH, 

	WM_RESOURCE_REMOVE,

	WM_PRIVATE_CALL_REJECTED
};



#endif // !defined(AFX_RADIOTYPES_H__7CF74587_3CFF_4ED7_9896_D96A662D6A40__INCLUDED_)
