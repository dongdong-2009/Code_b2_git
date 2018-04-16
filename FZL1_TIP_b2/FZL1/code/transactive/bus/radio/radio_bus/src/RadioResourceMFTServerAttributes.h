/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/bus/radio/RadioBus/src/RadioResourceMFTServerAttributes.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef RADIO_RESOURCE_MFT_SERVER_ATTRIBUTES_H_INCLUDED
#define RADIO_RESOURCE_MFT_SERVER_ATTRIBUTES_H_INCLUDED

enum AssociatedSpeaker
{
	NIL,

	SELECTED_SPEAKER,

	UNSELECTED_SPEAKER
};


enum PrivateCallState
{
	PC_NOT_INVOLVED,

	PC_INCOMING,

	PC_OUTGOING,

	PC_ESTABLISHED
};


enum AmbienceListeningCallState
{
	ALC_NOT_INVOLVED,

	ALC_INCOMING,

	ALC_ESTABLISHED
};

enum GroupConnectionState
{
	NAG,

	RELEASED,
    
    CONNECTION_BUSY, // either connecting / releasing

	CONNECTED
};

enum RadioEmergencyModeType
{
	RE_NO_EMERGENCY,

	RE_EMERGENCY,

	RE_UNACK_EMERGENCY
};


enum IscsState
{
	ISCS_NORMAL_MODE = 0,

	ISCS_DEGRADED_MODE = 1
};

enum RadioState
{
	RADIO_NORMAL_MODE = 2,

	RADIO_DEGRADED_MODE = 3
};

struct RadioResourceMFTServerAttributes
{
	public:
        RadioResourceMFTServerAttributes() :
            isResourceAssigned(false),
            bIsProfile(false),
            buttonOffset(0),
            associatedSpeaker(NIL),
            selectSpeakerVolume(0),
            unselectSpeakerVolume(0),
            isGroupConnected(NAG),
            isCurrentlyActive(false),
            privateCallState(PC_NOT_INVOLVED),
            ambienceListeningCallSate(ALC_NOT_INVOLVED),
            emergencyMode(RE_NO_EMERGENCY),
            emergencyRadioSSI(0),
			isPatchInvolved(false)
        {
        };

		/**
		 * Whether the resource has been associated with the CIE.
		 * Not applicable for 'connectable' resources.
		 */
		bool isResourceAssigned;

		/**
		 * Which group does this belong to, ie. profile or user.
		 */
		bool bIsProfile;
		/**
		 * Offset within its button list.
		 */
		unsigned int buttonOffset;

		AssociatedSpeaker associatedSpeaker;

		/**
		 * 0: Invalid/Muted/Unknown
		 * 1-8: valid values
		 */
		unsigned int selectSpeakerVolume;
		/**
		 * 0: Invalid/Muted/Unknown
		 * 1-8: valid values
		 */
		unsigned int unselectSpeakerVolume;

		GroupConnectionState isGroupConnected;
		/**
		 * Set when any (suitable) indication of activity detected.
		 */
		bool isCurrentlyActive;

		PrivateCallState privateCallState;

		AmbienceListeningCallState ambienceListeningCallSate;

		RadioEmergencyModeType emergencyMode;
		
		unsigned int emergencyRadioSSI;

		// TD14133 indicates the Transactive ID of the patch group 
		// this entity is a member of
		unsigned int isPatchInvolved;
};
#endif
