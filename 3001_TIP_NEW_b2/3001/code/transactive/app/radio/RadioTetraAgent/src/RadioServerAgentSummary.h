/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioServerAgentSummary.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Facade to transActive framework functionality.
 */

#ifndef RADIO_SERVER_AGENT_SUMMARY_H_INCLUDED
#define RADIO_SERVER_AGENT_SUMMARY_H_INCLUDED


#include "core/process_management/src/CtaRuntime.h"
#include "core/message/src/CtaAuditing.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_IRS_App
{

class RadioServerAgentSummary
{
public:
	RadioServerAgentSummary(TA_Base_Core::IEntityDataPtr agentConfiguration);

	virtual ~RadioServerAgentSummary();

    /**
	  * Helper to get all the parameters for the agent
      */
    bool loadCADIParameters();

    std::string getCADIHost();
    std::string getCADILogin();
    std::string getCADIPassword();
    std::string getCADIEncryption();

private:
    // disabled methods
    RadioServerAgentSummary(const RadioServerAgentSummary&);
    void operator=(const RadioServerAgentSummary&);

private:
	TA_Base_Core::CtaRuntime m_runtime;
	TA_Base_Core::CtaAuditing m_auditing;

    // entity configuration parameter values
    std::string m_CADIHost;
    std::string m_CADILogin;
    std::string m_CADIPassword;
    std::string m_CADIEncryption;

public:
    
    // entity configuration parameter names
    static const std::string CADI_HOST;
    static const std::string CADI_LOGIN;
    static const std::string CADI_PASSWORD;
    static const std::string CADI_ENCRYPTION;
};
}
#endif

