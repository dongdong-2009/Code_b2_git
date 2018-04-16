/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommanProcessorLib\src\ISDSMessageObserver.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/14/08 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is used for processing of the incoming commands.
  */

#ifndef _RADIO_DISPATCHER_H_
#define _RADIO_DISPATCHER_H_
#include <vector>
#include "app/radio/radio_simulator/CommandProcessorLib/src/ISDSMessageObserver.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/ICommandProcessor.h"
#include "boost/shared_ptr.hpp"


namespace TA_IRS_App
{
	class RadioDispatcher : public ISDSMessageObserver
	{
	public:
		/**
		 * Constructor
		 */
		RadioDispatcher();
		/**
		 * Destructor
		 */
		~RadioDispatcher();
		
		/**
		 * processSDSMessage
		 *
		 * This method is used process the incoming command from the RadioTetraAgent.
		 *
		 * @param : boost::shared_ptr<SDSMessageData>& sdsMessage
		 */
		void processSDSMessage(boost::shared_ptr<SDSMessageData>& sdsMessage);

		/**
		 * attachCommandProcessor
		 *
		 * This method is used to attach a command processor observer
		 *
		 * @param : ICommandProcess* commandProcessor
		 */
		void attachCommandProcessor(ICommandProcessor* commandProcessor);

		/**
		 * detachCommandProcessor
		 *
		 * This method is used to detach a command processor observer
		 *
		 * @param : ICommandProcess* commandProcessor
		 */
		void detachCommandProcessor(ICommandProcessor* commandProcessor);

	private:
		/**
		 * Disable the assignment and copy constructor
		 */
		RadioDispatcher& operator= ( const RadioDispatcher&);
		RadioDispatcher ( const RadioDispatcher& );

		std::vector<ICommandProcessor*> m_commandProcessor;
	};
}

#endif