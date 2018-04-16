/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\TTISCommandProcessor.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/16/08 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is used for TTIS command processing.
  */

#ifndef _TTIS_COMMAND_PROCESSOR_H_
#define _TTIS_COMMAND_PROCESSOR_H_
#include "app/radio/radio_simulator/CommandProcessorLib/src/ICommandProcessor.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/TrainCommandProcessorThread.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/IPACMData.h"

#include "app/radio/radio_simulator/TrainSimulator/IModel.h"


namespace TA_IRS_App
{
	 class TTISCommandProcessor : public ICommandProcessor,
						        public TrainCommandProcessorThread< class SDSMessageData >
	{
	public:
		/**
		 * Constructor
		 */
		TTISCommandProcessor( IPACMData& paCMData );
		/**
		 * Destructor
		 */
		virtual ~TTISCommandProcessor();

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

		std::string processorName() const { return "TTISCommandProcessor"; }

		void attachSocketManager(IModel* model);



	private:
		/**
		 * Disable copy and assignment constructor
		 */
		TTISCommandProcessor (const TTISCommandProcessor& );
		TTISCommandProcessor& operator= (const TTISCommandProcessor);

		static int _commandProtocol[5];
		IModel* m_model;
	
	};
}
#endif