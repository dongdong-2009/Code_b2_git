

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADICommand.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */

#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADICommand.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventManager.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADICommandAdapter.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADITypes.h"
#include "core/utilities/src/DebugUtil.h"
#include <sstream>


namespace TA_IRS_App {
	
	/************************************************************************/
	/* Function																*/
	/************************************************************************/

	CADIFunction::CADIFunction()
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	CADIFunction::~CADIFunction()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

    static void LogFunctionError(const std::string& funcName, TokenList& args)
    {
        std::ostringstream logStr;
        logStr << funcName << " incorrectly called with " << args.size() << " arguments:" << std::endl;
        for (TokenList::iterator it = args.begin(); it != args.end(); ++it)
        {
            logStr << *it << std::endl;
        }
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, logStr.str().c_str());
    }


	/************************************************************************/
	/* Handshake                                                            */
	/************************************************************************/

	HandshakeCommand::HandshakeCommand()
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	HandshakeCommand::~HandshakeCommand()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int HandshakeCommand::call(TokenList &args)
	{
		FUNCTION_ENTRY("call");
		
		if(args.size() != 1)
		{
            LogFunctionError("HandshakeCommand", args);
			return false;
		}
        
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "HandshakeCommand[%s]", args[0].c_str());

		bool status = true;
		try
		{
			TokenList::iterator i = args.begin();

			// first param
			std::string hostname = *i;

			if (cadi_handshake(hostname.c_str()) == CADI_SUCCESS)
            {
			    cadiEventManager::instance()->startEventListener();
            }
            else
            {
                status = false;
            }
        
		}
		catch(...)
		{
			status = false;
		}

        if (status == false)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "HandshakeCommand failed");
        }

		FUNCTION_EXIT;
		return status;
	}


	/************************************************************************/
	/* Login                                                                */
	/************************************************************************/

	LoginCommand::LoginCommand()
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	LoginCommand::~LoginCommand()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int LoginCommand::call(TokenList &args)
	{
		FUNCTION_ENTRY("call");

		if (args.size() != 3)
		{
            LogFunctionError("Login", args);
			return false;
		}

		bool status = true;
		try
		{
			TokenList::iterator i = args.begin();

			// first param
			std::string username = *i;

			// second param
			++i;
			std::string password = *i;

			// third param
			++i;
			int encrypt = (*i == "1" ? 1 : 0);

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "LoginCommand[user: %s pass: %s encrypt: %s]", username.c_str(), password.c_str(), args[2].c_str());

			int ret = cadi_login(username.c_str(), password.c_str(), encrypt);
            status = (ret == CADI_SUCCESS);

            if (!status)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "LoginCommand failed : %d", ret);
            }
		}
		catch (...)
		{
			status = false;
		}

		FUNCTION_EXIT;
		return status;
	}


	/************************************************************************/
	/* Logout                                                               */
	/************************************************************************/

	LogoutCommand::LogoutCommand()
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	LogoutCommand::~LogoutCommand()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int LogoutCommand::call(TokenList &args)
	{
		FUNCTION_ENTRY("call");
		if (args.size() != 0)
		{
            LogFunctionError("LogoutCommand", args);
			return false;
		}

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "LogoutCommand[]");

		bool status = true;
		status = cadi_logout() == CADI_SUCCESS;

		FUNCTION_EXIT;
		return status;
	}


	/************************************************************************/
	/* Regroup                                                              */
	/************************************************************************/

	RegroupCommand::RegroupCommand()
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	RegroupCommand::~RegroupCommand()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int RegroupCommand::call(TokenList &args)
	{
		
		FUNCTION_ENTRY("call");
		if (args.size() != 2)
		{
            LogFunctionError("RegroupCommand", args);
			return 0;
		}

		int status = 0; // TD16814

		try 
		{
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "RegroupCommand[radio: %s group: %s]", args[0].c_str(), args[1].c_str());

			TokenList::iterator i = args.begin();

			// first param
			std::stringstream p1;
			p1 << *i << std::ends;
			unsigned long radio;
			p1 >> radio;

			// second param
			++i;
			std::stringstream p2;
			p2 << *i << std::ends;
			unsigned long group;
			p2 >> group;

			// assign specific return value to determine error code
			// a non-zero positive integer denotes a successful operation
			status = cadi_regroup(radio, group); 

			// TD16814
			std::string logStr;
			switch(status)
			{
				case -1: logStr = "The command cannot be granted for a variety of reasons.";break;
				case -2: logStr = "The CADI client must first log on before any command can be sent."; break;
				case -3: logStr = "The zone controller is disabled, unreachable, or in idle state."; break;
				case -4: logStr = "The radio ID given is unknown (not in the database)."; break;
				case -5: logStr = "The command type is unknown."; break;
				case -6: logStr = "The target ID is not valid in a regroup."; break;
				case -7: logStr = "The CADI client user does not have permission to operate on the radio.";
			}

			if (status < 0)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, logStr.c_str());
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CADI_REGROUP command status => %d",status);
			}
			// TD16814
			
		} 
		catch(...)
		{
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "Exception occurred on 'cadi_regroup' command");

		}

		FUNCTION_EXIT;
		return status;
	}



	/************************************************************************/
	/* Cancel Regroup                                                       */
	/************************************************************************/

	CancelRegroupCommand::CancelRegroupCommand()
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	CancelRegroupCommand::~CancelRegroupCommand()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int CancelRegroupCommand::call(TokenList & args)
	{
		FUNCTION_ENTRY("call");
		if (args.size() != 1)
		{
            LogFunctionError("CancelRegroupCommand", args);
			return false;
		}


		bool status = true;
		try 
		{
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "CancelRegroupCommand[radio: %s]", args[0].c_str());

			TokenList::iterator i = args.begin();

			// first param
			std::stringstream p1;
			p1 << *i << std::ends;
			unsigned long radio;
			p1 >> radio;

			status = cadi_cancelRegroup(radio) == CADI_SUCCESS;
		} 
		catch (...)
		{
			status = false;
		}
		FUNCTION_EXIT;
		return status;
	}


	/************************************************************************/
	/* XRegroupWGid                                                         */
	/************************************************************************/

	XRegroupWGidCommand::XRegroupWGidCommand()
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	XRegroupWGidCommand::~XRegroupWGidCommand()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int XRegroupWGidCommand::call(TokenList & args)
	{
		FUNCTION_ENTRY("call");
		if (args.size() != 2)
		{
            LogFunctionError("XRegroupWGidCommand", args);
			return 0;
		}


		int status = 0;
		try 
		{
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "XRegroupWGidCommand[radio: %s group: %s]", args[0].c_str(), args[1].c_str());
			TokenList::iterator i = args.begin();

			// first param
			std::stringstream p1;
			p1 << *i << std::ends;
			unsigned long radio;
			p1 >> radio;

			// second param
			++i;
			std::stringstream p2;
			p2 << *i << std::ends;
			unsigned long group;
			p2 >> group;

			status = cadi_cancelRegroupWGid(radio, group);

			// TD16814
			std::string logStr;
			switch(status)
			{
				case -1: logStr = "The command cannot be granted for a variety of reasons.";break;
				case -2: logStr = "The CADI client must first log on before any command can be sent."; break;
				case -3: logStr = "The zone controller is disabled, unreachable, or in idle state."; break;
				case -4: logStr = "The radio ID given is unknown (not in the database)."; break;
				case -5: logStr = "The command type is unknown."; break;
				case -6: logStr = "The target ID is not valid in a regroup."; break;
				case -7: logStr = "The CADI client user does not have permission to operate on the radio.";
			}

			if (status < 0)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, logStr.c_str());
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CADI_CANCELREGROUP_W_GID command status => %d",status);
			}
			// TD16814

		} 
		catch (...)
		{
			status = 0;
		}
		FUNCTION_EXIT;
		return status;
	}

}
