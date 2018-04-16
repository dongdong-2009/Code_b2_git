/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommanProcessorLib\src\ISDSMessageObserver.cpp $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/14/08 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is used for processing of the incoming commands.
  */

#include "app/radio/radio_simulator/CommandProcessorLib/src/RadioDispatch.h"
#include "core/utilities/src/DebugUtil.h"
#include <algorithm>

namespace TA_IRS_App
{
	RadioDispatcher::RadioDispatcher()
	{
		FUNCTION_ENTRY("RadioDispatcher");
		FUNCTION_EXIT;
	}

	RadioDispatcher::~RadioDispatcher()
	{
		FUNCTION_ENTRY("~RadioDispatcher");
		FUNCTION_EXIT;
	}

	void RadioDispatcher::processSDSMessage(boost::shared_ptr<SDSMessageData>& sdsMessage)
	{
		FUNCTION_ENTRY("processSDSMessage");
		
		std::vector<ICommandProcessor*>::iterator iter = m_commandProcessor.begin();
		for ( iter; iter != m_commandProcessor.end(); iter++)
		{
			//Notify all ICommand Observers
			(*iter)->sendSDSMessage(sdsMessage);
		}

		FUNCTION_EXIT;
	}

	void RadioDispatcher::attachCommandProcessor(ICommandProcessor* commandProcessor)
	{
		FUNCTION_ENTRY("attachCommandProcessor");

		m_commandProcessor.push_back(commandProcessor);

		FUNCTION_EXIT;
	}

	void RadioDispatcher::detachCommandProcessor(ICommandProcessor* commandProcessor)
	{
		FUNCTION_ENTRY("detachCommandProcessor");

		std::vector<ICommandProcessor*>::iterator iter;
		iter = std::find( m_commandProcessor.begin(), m_commandProcessor.end(), commandProcessor);

		if ( iter != m_commandProcessor.end())
		{
			//Found it
			m_commandProcessor.erase(iter);
		}

		FUNCTION_EXIT;
	}
}