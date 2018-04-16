/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioServerAgentSummary.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/RadioTetraAgent/src/RadioServerAgentSummary.h"

#include "core/message/types/MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_IRS_App;
const std::string RadioServerAgentSummary::CADI_HOST = "CADIHost";
const std::string RadioServerAgentSummary::CADI_LOGIN = "CADILogin";
const std::string RadioServerAgentSummary::CADI_PASSWORD = "CADIPassword";
const std::string RadioServerAgentSummary::CADI_ENCRYPTION = "CADIEncryption";


RadioServerAgentSummary::RadioServerAgentSummary( TA_Base_Core::IEntityDataPtr agentConfiguration)
:	m_runtime(agentConfiguration),
	m_auditing(m_runtime.getMyEntityKey(), *(new TA_Base_Core::MessageContext("channel","context",TA_Base_Core::AuditMessage)) )
{
	FUNCTION_ENTRY("constructor");

	FUNCTION_EXIT;
}

RadioServerAgentSummary::~RadioServerAgentSummary()
{
	FUNCTION_ENTRY("destructor");

	FUNCTION_EXIT;
}

bool RadioServerAgentSummary::loadCADIParameters()
{
    try
    {
        m_CADIHost = m_runtime.getConfigParameterValue(CADI_HOST);
        m_CADILogin = m_runtime.getConfigParameterValue(CADI_LOGIN);
        m_CADIPassword = m_runtime.getConfigParameterValue(CADI_PASSWORD);
        m_CADIEncryption = m_runtime.getConfigParameterValue(CADI_ENCRYPTION);
        return true;
    }
    catch(TA_Base_Core::TransactiveException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Cannot retrieve parameter for CADI");
    }
    return false;
}


std::string RadioServerAgentSummary::getCADIHost()
{
    return m_CADIHost;
}

std::string RadioServerAgentSummary::getCADILogin()
{
    return m_CADILogin;
}

std::string RadioServerAgentSummary::getCADIPassword()
{
    return m_CADIPassword;
}

std::string RadioServerAgentSummary::getCADIEncryption()
{
    return m_CADIEncryption;
}

