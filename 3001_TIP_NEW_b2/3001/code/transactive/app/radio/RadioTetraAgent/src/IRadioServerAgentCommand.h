/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/IRadioServerAgentCommand.h $
 * @author:  Anggiono
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef INTERFACE_RADIO_SERVER_AGENT_COMMAND_H_INCLUDED
#define INTERFACE_RADIO_SERVER_AGENT_COMMAND_H_INCLUDED


#include <string>
#include <vector>
#include "bus/radio/radio_bus/src/radiotypes.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_Data.h"


class IRadioServerAgentCommand
{
	public:

		virtual ~IRadioServerAgentCommand() {};

public:
    /** 
     * Used only by the MFT Agent to notify the Server Agent that the operator's
     * console has received a radio status message 
     */
	void virtual radioStatusMsg(unsigned long recipientSSI, unsigned long statusMsg) =0;

    /** 
     * Retrieve DynGroup configuration form Radio Directory DAI for each 
     * Radio Unit, perform CADI Regroup
     */
	void virtual connectDynamicRegroup(RadioResource dynamicGroup,unsigned long talkgroupID) =0;

    /**
     * Retrieve DynGroup configuration form Radio Directory DAI for each 
     * Radio Unit, perform CADI Cancel Regroup
     */
	void virtual releaseDynamicRegroup(RadioResource dynamicGroup,unsigned long talkgroupID) =0;
};
#endif
