/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentHelper.h $
 * @author:  Albertus Zakaria
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 11:47:25 $
 * Last modified by:  $Author: builder $
 * 
 */ 

#ifndef __RADIO_MFT_AGENT_HELPER_INCLU0234204382340_
#define __RADIO_MFT_AGENT_HELPER_INCLU0234204382340_


#include <string>
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "bus/radio/radio_agent/idl/src/CommonRadioAgentIDL.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

class RadioMFTAgentHelper
{
public:

    static std::string getRadioMFTAgentEntityName();

    static unsigned long getRadioMFTAgentEntityKey();


	/*
	 * Helper method to get the entity object of RadioMFTAgent
     * It is the caller's responsibility to delete the returned object.
     * The data returned is based on current console's location key
	 */
    static TA_Base_Core::IEntityData* getRadioMFTAgentEntity();


    static std::string getRadioServerAgentEntityName();

    static unsigned long getRadioServerAgentEntityKey();

	/*
	 * Helper method to get the entity object of RadioServerAgent
     * It is the caller's responsibility to delete the returned object.
     * The data returned is fixed to Occ_int entity
	 */
    static TA_Base_Core::IEntityData* getRadioServerAgentEntity();

    static void convertRadioResourceFromIDL(const TA_IRS_Bus::Radio::RadioResource& source, RadioResource& target);
    static void convertRadioResourceToIDL(const RadioResource& source, TA_IRS_Bus::Radio::RadioResource& target);
    static RadioResourceType convertResourceType(TA_IRS_Bus::Radio::ERadioResourceType etype);
    static TA_IRS_Bus::Radio::ERadioResourceType convertResourceType(RadioResourceType etype);
    static TA_IRS_Bus::Radio::EPrivateCallState convertPCState(PrivateCallState state);

/*    static void convertTelephoneFromIDL(const TA_IRS_Bus::Telephony::TelephoneResource& source, TA_IRS_Bus::TelephoneResource& target);
    static void convertTelephoneToIDL(const TA_IRS_Bus::TelephoneResource& source, TA_IRS_Bus::Telephony::TelephoneResource& target);
*/
	static std::string getProfileName(); // Fixed TD14320
	static int getBIMResourceID();

	static int getSPIResourceID(); // TD16770

	static std::string getRadioSystemIOR();
    static std::string getRadioSystemUsername();
    static std::string getRadioSystemPassword();
    static std::string getRadioSystemISSI();
	
private:
    // disabled methods
    RadioMFTAgentHelper();
    RadioMFTAgentHelper(const RadioMFTAgentHelper&);
    void operator=(const RadioMFTAgentHelper&);

public:
    static const std::string RADIO_MFT_AGENT_ENTITY_NAME;
    static const std::string RADIO_SERVER_AGENT_ENTITY_NAME;

};


#endif
