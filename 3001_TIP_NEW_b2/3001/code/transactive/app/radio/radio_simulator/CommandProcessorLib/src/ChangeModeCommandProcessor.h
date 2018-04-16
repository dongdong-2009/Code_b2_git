
#ifndef _CHANGEMODE_COMMAND_PROCESSOR_H_
#define _CHANGEMODE_COMMAND_PROCESSOR_H_
#include "app/radio/radio_simulator/CommandProcessorLib/src/ICommandProcessor.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/TrainCommandProcessorThread.h"

namespace TA_IRS_App
{
	class ChangeModeCommandProcessor : public ICommandProcessor,
									   public TrainCommandProcessorThread<class SDSMessageData>
	{
	public:
		/**
		 * Constructor
		 */
		ChangeModeCommandProcessor( IPACMData& paCMData );

		/**
		 * Destructor
		 */
		virtual ~ChangeModeCommandProcessor();

		/**
		 * ICommandProcessor Interface
		 */
		void sendSDSMessage(boost::shared_ptr<SDSMessageData>& sdsMessage);

		/**
		 * TrainCommandProcessorThread Interface
		 */
		void processTrainCommand(boost::shared_ptr<SDSMessageData>& sdsMessage);

		std::string processorName()const { return "ChangeModeCommand"; }

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		ChangeModeCommandProcessor(const ChangeModeCommandProcessor& );
		ChangeModeCommandProcessor& operator= (const ChangeModeCommandProcessor& );
	};
}

#endif