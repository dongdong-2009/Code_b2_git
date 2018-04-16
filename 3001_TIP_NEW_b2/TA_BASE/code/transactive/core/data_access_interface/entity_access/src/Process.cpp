/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/Process.cpp $
  * @author Rob Ashcroft
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Process is a concrete implementation of IProcess, which is in turn an implementation of
  * IEntityData. It proves read-only access to Process entity data, and a specific framework
  * for Process entity parameters.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER


#include "core/data_access_interface/entity_access/src/Process.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    
    const std::string Process::ENTITY_TYPE      = "MonitoredProcess";
    // The entity parameters as they appear in the Database
    const std::string Process::USERNAME = "UserName";
	const std::string Process::PASSWORD = "Passwd";
    const std::string Process::MANAGEDPROCESS = "ManagedProcess";
    const std::string Process::SERVICEPROCESS = "ServiceProcess";
    const std::string Process::STARTLAG = "StartLag";
    const std::string Process::TERMINATETIMEOUT = "TerminateTimeout";
    const std::string Process::KILLTIMEOUT = "KillTimeout";
    const std::string Process::STARTUPTIMEOUT = "StartTimeout";
    const std::string Process::CONTROLTIMEOUT = "ControlTimeout";
    const std::string Process::MONITORTIMEOUT = "MonitorTimeout";
    const std::string Process::AUTOFAILBACK = "AutoFailback";
	const std::string Process::PRIORITY = "Priority";
	const std::string Process::PROCESSOR = "Processor";

	// A constant for the PROCESSOR entity parameter.
	const int Process::NOT_BOUND = -1;


	Process::Process()
	{
	}


    Process::Process(unsigned long key) :
		EntityData( key, getStaticType() )
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "Process (constructor)");
        LOG( SourceInfo, DebugUtil::FunctionExit, "Process (constructor)");
    }


    Process::~Process()
    {
    }

    
    std::string Process::getType()
    {
        return getStaticType();
    }


    std::string Process::getStaticType()
    {
        return ENTITY_TYPE;
    }


    bool Process::isManagedProcess()
    {
        bool isManagedProcess = false;
		std::string value = getHelper()->getParameter(MANAGEDPROCESS);
		if(!value.empty())
		{
			isManagedProcess = EntityHelper::getBooleanData(value);
		}

        return isManagedProcess;
    }


    std::string Process::getUserName()
    {
		return getHelper()->getParameter(USERNAME);
    }

	std::string Process::getPassWord()
    {
        return getHelper()->getParameter(PASSWORD);
    }
    unsigned int Process::getKillTimeout()
    {
		unsigned int killTimeout = 0;
		std::string value = getHelper()->getParameter(KILLTIMEOUT);
		try
		{
			killTimeout = EntityHelper::getIntegerData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used.
		}

        return killTimeout;
    }

    unsigned int Process::getTerminateTimeout()
    {
		unsigned int terminateTimeout;
		std::string value = getHelper()->getParameter(TERMINATETIMEOUT);
		try
		{
			terminateTimeout = EntityHelper::getIntegerData(value);
		}
		catch (DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used.
		}
        return terminateTimeout;
    }

    unsigned int Process::getStartLag()
    {
		unsigned int startLag = 0;
		std::string value = getHelper()->getParameter(STARTLAG);
		if(!value.empty())
		{
			startLag = EntityHelper::getIntegerData(value);
		}
        return startLag;
    }

    unsigned int Process::getStartupTimeout()
    {
		unsigned int startupTimeout = 0;
		std::string value = getHelper()->getParameter(STARTUPTIMEOUT);
		try
		{
			startupTimeout = EntityHelper::getIntegerData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used.
		}

        return startupTimeout;
    }

    unsigned int Process::getChangeControlTimeout()
    {
        unsigned int changeControlTimeout = 0;
		std::string value = getHelper()->getParameter(CONTROLTIMEOUT);
		try
		{
			changeControlTimeout = EntityHelper::getIntegerData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used.
		}

        return changeControlTimeout;
    }

    unsigned int Process::getChangeMonitorTimeout()
    {
        unsigned int changeMonitorTimeout = 0;
		std::string value = getHelper()->getParameter(MONITORTIMEOUT);
		try
		{
			changeMonitorTimeout = EntityHelper::getIntegerData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used.
		}

        return changeMonitorTimeout;
    }


    bool Process::isServiceProcess()
    {
        bool isServiceProcess = false;

		std::string value = getHelper()->getParameter(SERVICEPROCESS);
		try
		{
			isServiceProcess = EntityHelper::getBooleanData(value);
		}
		catch (DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

        return isServiceProcess;
    }
    
    bool Process::shouldAutoFailback()
    {
        bool shouldAutoFailback = false;
		std::string value = getHelper()->getParameter(AUTOFAILBACK);
		try
		{
			shouldAutoFailback = EntityHelper::getBooleanData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}
        return shouldAutoFailback;
    }

	int Process::getPriority()
	{
		int priority = -1;
		std::string value = getHelper()->getParameter(PRIORITY);
		try
		{
			priority = EntityHelper::getIntegerData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}
		return priority;
	}
 
	int Process::getBoundProcessor()
	{
		int processor = NOT_BOUND;
		std::string value = getHelper()->getParameter(PROCESSOR);
		try
		{
			processor = EntityHelper::getIntegerData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}
		return processor;
	}

    IEntityData* Process::clone(unsigned long key)
    {
        return new Process(key);        
    }


    void Process::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core
