/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioSDSSummary.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Facade to transActive framework functionality.
 */

#ifndef RADIO_SDS_SUMMARY_H_INCLUDED
#define RADIO_SDS_SUMMARY_H_INCLUDED


#include "core/process_management/src/CtaRuntime.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include <map>
namespace TA_IRS_App
{
class RadioSDSSummary
{
public:
	RadioSDSSummary(TA_Base_Core::IEntityDataPtr agentConfiguration);

	virtual ~RadioSDSSummary();

    /**
	  * Helper to get all the parameters for the agent
      */
    bool loadSDSParameters();

    std::string getSDTSHost();
    std::string getSDTSPort();
    unsigned long getSDTSClientSSI();
    unsigned long getSDTSPassNumber();
	std::string getMFTName();

	std::string getSDTSSecondaryHost();
	std::string getSDTSSecondaryPort();

	unsigned long   getEntityKey();
	unsigned long   getSubsystem();
	unsigned long  getLocation();
	unsigned long  getEntityTypeKey();
	std::string		getEntityName();
	std::string getSDTSLinkDpName();
	unsigned int getSDSMaxNumRetry();


	struct  SDSConsoleData 
	{
		unsigned long clientSSI;
		unsigned long passNumber;
	};

	typedef std::map<std::string,SDSConsoleData> SDSConsoleDataMap;
	SDSConsoleDataMap getConsoleDataMap();

private:
    // disabled methods
    RadioSDSSummary(const RadioSDSSummary&);
    void operator=(const RadioSDSSummary&);

	//SDSConsoleData getMFTData(const std::string& console); 

private:
	TA_Base_Core::CtaRuntime m_runtime;

    // entity configuration parameter values
    std::string m_SDTSHost;
    std::string m_SDTSPort;
    std::string m_SDTSClientSSI;
    std::string m_SDTSPassNumber;
	std::string m_MFTName;

	std::string m_SDTSSecondaryHost;
	std::string m_SDTSSecondaryPort;
	std::string m_SDTSLinkDpName;
	std::string m_SDSMaxNumRetry;
	SDSConsoleDataMap m_consoleDataMap;

public:
    
    // entity configuration parameter names
    static const std::string SDTS_HOST;
    static const std::string SDTS_PORT;
    static const std::string SDTS_CLIENT_SSI;
    static const std::string SDTS_PASS_NUMBER;
	static const std::string MFT_NAME;
	
	static const std::string SDTS_SECONDARY_HOST;
	static const std::string SDTS_SECONDARY_PORT;
	static const std::string SDTS_LINK_DP_NAME;
	static const std::string SDTS_MAX_NUM_RETRY;
};
}
#endif

