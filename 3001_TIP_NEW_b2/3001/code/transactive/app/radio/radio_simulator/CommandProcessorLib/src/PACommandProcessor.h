/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\PACommandProcessor.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/16/08 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is used for PA command processing.
  */

#ifndef _PA_COMMAND_PROCESSORS_H_
#define _PA_COMMAND_PROCESSORS_H_
#include "app/radio/radio_simulator/CommandProcessorLib/src/ICommandProcessor.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/TrainCommandProcessorThread.h"

#include "..\..\TrainSimulator\EtcTrainSimConfigHandler.h"
#include "..\..\TrainSimulator\AutoSendDataModel.h"
namespace TA_IRS_App
{
	class PACommandProcessor : public ICommandProcessor,
						        public TrainCommandProcessorThread< class SDSMessageData >
	{
	public:
		/**
		 * Constructor
		 */
		PACommandProcessor( IPACMData& paCMData );
		/**
		 * Destructor
		 */
		virtual ~PACommandProcessor();

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

		/**
		 * buildPALiveRequestResponse
		 *
		 * This method is used to response the Live Request Reset
		 *
		 * @param : const std::vector<unsigned char>& pData
		 */
		void buildPALiveRequestResetResponse(const std::vector<unsigned char>& pData);

		std::string processorName() const { return "PACommandProcessor"; }

		void buildPAReadyResponse(const std::vector<unsigned char>& pData);
		void buildLivePaReset(const std::vector<unsigned char>& pData, bool badRespons = false);
		void buildCommandReceived(const std::vector<unsigned char>& pData, bool badResponse = false);
		void buildPAMessageCompleteResponse(const std::vector<unsigned char>& pData, unsigned char anouncementid);
		void buildPreRecordedPaTimeOutResponse(const std::vector<unsigned char>& pData, std::vector<unsigned char>& resp);

		void processSecondResponse(const std::vector<unsigned char>& pData);

		
	private:
		/**
		 * Disable copy and assignment constructor
		 */
		PACommandProcessor (const PACommandProcessor& );
		PACommandProcessor& operator= (const PACommandProcessor);

		
		static long m_lTrainCount;
		bool m_bMaxReach;
	};
}
#endif