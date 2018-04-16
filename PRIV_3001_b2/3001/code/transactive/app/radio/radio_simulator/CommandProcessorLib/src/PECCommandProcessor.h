/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\PECCommandProcessor.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/16/08 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is used for PEC command processing.
  */

#ifndef _PEC_COMMAND_PROCESSOR_H_
#define _PEC_COMMAND_PROCESSOR_H_
#include "app/radio/radio_simulator/CommandProcessorLib/src/ICommandProcessor.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/TrainCommandProcessorThread.h"



namespace TA_IRS_App
{
	class PECCommandProcessor : public ICommandProcessor,
						        public TrainCommandProcessorThread< class SDSMessageData >
	{
	public:
		/**
		 * Constructor
		 */
		PECCommandProcessor(IPACMData& paCMData);
		/**
		 * Destructor
		 */
		virtual ~PECCommandProcessor();

		/**
		 * sendSDSMessage
		 *
		 * This method is used to process the SDS Message 
		 *
		 * @param boost::shared_ptr<SDSMessageData>& sdsMessaga
		 */
		void sendSDSMessage(boost::shared_ptr<SDSMessageData>& sdsMessage);

		/**
		 * processTrainCommand
		 *
		 * Process the item in the queue, implemented by child classes to provide
		 * implementation specific processing.
		 * 
		 * @param: boost::shared_ptr<T>& sdsMessage
		 */
		void processTrainCommand(boost::shared_ptr<SDSMessageData>& sdsMessage);

		std::string processorName() const { return "PECCommandProcessor"; }

		void buildPecReadyResponse(const std::vector<unsigned char>& pData);

		void buildCommandReceived(int userDataIndex, std::vector<unsigned char>& resp, const std::vector<unsigned char>& pData);

		void processSecondResponse(const std::vector<unsigned char>& pData);

		

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		PECCommandProcessor (const PECCommandProcessor& );
		PECCommandProcessor& operator= (const PECCommandProcessor);
		

	};
}
#endif