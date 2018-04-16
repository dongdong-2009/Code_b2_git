/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/process_management/src/UtilityInitialiser.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#include "StdAfx.h"
#include "core/utilities/src/DebugSETranslator.h"
#include "core/process_management/src/UtilityInitialiser.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtilInit.h"
#include "core/utilities/src/TAAssert.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/src/UtilitiesException.h"
#include "core/exceptions/src/InvalidCommandLineException.h"
#include "core/data_access_interface/src/NamingPortAccessFactory.h"

namespace
{
    std::string makeCommandLine(int argc, char* argv[])
    {
        std::stringstream strm;

        for (int i = 0; i < argc; ++i)
        {
            strm << (0 == i ? "" : " ") << argv[i];
        }

        return strm.str();
    }
}

namespace TA_Base_Core
{

    bool UtilityInitialiser::initialiseUtilities(int argc, char* argv[])
    {
        return initialiseUtilities(makeCommandLine(argc, argv));
    }

    bool UtilityInitialiser::initialiseUtilities(const std::string& commandLine)
    {
        try
        {
            if (false == RunParams::getInstance().parseCmdLine(commandLine.c_str()))
            {
                if (RunParams::getInstance().isSet(RPARAM_VERSION))
                {
                    return false;
                }

                TA_THROW(InvalidCommandLineException("Command line arguments are in invalid format"));
            }

            parseLocalConfigurationFile();
            gSetDebugUtilFromRunParams();
            gInitStructuredExceptionHandler();
            initialiseCorba();
        }
        catch (const UtilitiesException&)
        {
            throw;
        }
        catch (const InvalidCommandLineException&)
        {
            throw;
        }
        catch (...)
        {
            TA_THROW(UtilitiesException("One or more utilities failed to initialise"));
        }

        return true;
    }

    void UtilityInitialiser::initialiseCorba()
    {
        int port = getRunParamValue(RPARAM_CORBA_NAMING_PORT, 0);

        if (false == CorbaUtil::getInstance().initialise(port))
        {
            TA_THROW(UtilitiesException("Failed to initialise corba"));
        }

        try
        {
            CorbaUtil::getInstance().activate();
        }
        catch (...)
        {
            TA_THROW(UtilitiesException("Failed to initialise corba"));
        }
    }
}
